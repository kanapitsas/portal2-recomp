// Natif installer: a window around ./setup.sh for people who do not use
// Terminal. It carries a snapshot of the repository (Resources/source.tar.gz,
// made by tools/build_installer.sh), unpacks it in Application Support, finds
// Portal 2 and Half-Life 2 the way setup.sh does, then runs setup.sh once per
// game the user ticked and shows its steps. It never downloads a game: the
// Steam console does, by the user's hand.
//
//   installer --snapshot out.png [state]   renders the window off screen
//   installer --detect                     prints what it finds, then exits
import AppKit
import SwiftUI

// $HOME first (the same folder for a Finder app), so tests can point elsewhere.
let home = ProcessInfo.processInfo.environment["HOME"].map { URL(fileURLWithPath: $0) }
    ?? FileManager.default.homeDirectoryForCurrentUser
// Source and build of both games (~5 GB each), apart from their saves.
// The folder name is the first release's (Portal 2 only), kept so that an
// update rebuilds incrementally; uninstall.sh trashes it on its own yes.
let sourceDir = home.appendingPathComponent("Library/Application Support/Portal 2 Natif Build")
let steamSupport = home.appendingPathComponent("Library/Application Support/Steam")
let logFile = home.appendingPathComponent("Library/Logs/NatifInstaller.log")

// MARK: - Games (tools/games.py, setup.sh)

struct Game: Identifiable, Equatable {
    let id: String              // setup.sh --game
    let title: String
    let appID: Int
    let dataDepot: Int          // depot with the game data, separate from the Linux binaries
    let linuxDepot: Int
    let linuxSize: String
    let marker: String          // folder inside the game folder
    let markerFile: String      // file that only a complete install has (Steam can leave an empty marker folder)
    let installFolder: String   // under steamapps/common
    let linuxFiles: [String]    // files of the Linux depot (setup.sh's checks)
    let minutes: Int

    var appName: String { "\(title) Natif" }
    var installedApp: URL { home.appendingPathComponent("Applications/\(appName).app") }
    var depotCommand: String { "download_depot \(appID) \(linuxDepot)" }

    func isGameFolder(_ url: URL) -> Bool {
        FileManager.default.fileExists(atPath: url.appendingPathComponent(markerFile).path)
    }
    func dataIssue(_ url: URL) -> String? {
        guard id == "portal2" else { return nil }
        let relative = "portal2_dlc2/scripts/editoritems.txt"
        let file = url.appendingPathComponent(relative)
        let info = try? file.resolvingSymlinksInPath().resourceValues(forKeys: [.isRegularFileKey, .fileSizeKey])
        guard info?.isRegularFile == true, (info?.fileSize ?? 0) > 0,
              FileManager.default.isReadableFile(atPath: file.path), isDir(url.appendingPathComponent("platform")) else {
            return "Incomplete game data: check \(relative) (missing, unreadable or empty) and the platform folder. An empty placeholder does not help. In Steam, verify the integrity of Portal 2's files (Properties > Installed Files), choose a complete game folder, or download the game data below:"
        }
        return nil
    }
    func isLinuxDepot(_ url: URL) -> Bool {
        linuxFiles.allSatisfy { FileManager.default.fileExists(atPath: url.appendingPathComponent($0).path) }
    }
    func findGame() -> URL? {
        let installed = steamSupport.appendingPathComponent("steamapps/common/\(installFolder)")
        let candidates = [installed, findDepot(appID, dataDepot)].compactMap { $0 }.filter(isGameFolder)
        return candidates.first(where: { dataIssue($0) == nil }) ?? candidates.first
    }
    func findLinux() -> URL? {
        if let depot = findDepot(appID, linuxDepot), isLinuxDepot(depot) { return depot }
        return nil
    }
}

let allGames = [
    Game(id: "portal2", title: "Portal 2", appID: 620, dataDepot: 621, linuxDepot: 661, linuxSize: "about 90 MB",
         marker: "portal2", markerFile: "portal2/pak01_dir.vpk", installFolder: "Portal 2",
         linuxFiles: ["bin/linux32/engine.so", "portal2/bin/linux32/server.so"], minutes: 10),
    Game(id: "hl2", title: "Half-Life 2", appID: 220, dataDepot: 221, linuxDepot: 224, linuxSize: "about 320 MB",
         marker: "hl2_complete", markerFile: "hl2_complete/gameinfo.txt", installFolder: "Half-Life 2",
         linuxFiles: ["hl2_linux", "bin/engine.so", "hl2_complete/bin/server.so"], minutes: 20),
]

// MARK: - Detection (same rules as setup.sh)

func isDir(_ url: URL) -> Bool {
    var dir: ObjCBool = false
    return FileManager.default.fileExists(atPath: url.path, isDirectory: &dir) && dir.boolValue
}

/// Folder written by the Steam console's download_depot (setup.sh's find_depot).
func findDepot(_ app: Int, _ id: Int) -> URL? {
    guard isDir(steamSupport) else { return nil }
    let out = run("/usr/bin/find", [steamSupport.path, "-maxdepth", "9", "-type", "d",
                                    "-path", "*/content/app_\(app)/depot_\(id)"]).output
    return out.split(separator: "\n").first.map { URL(fileURLWithPath: String($0)) }
}

/// Command line tools or Xcode selected, with a clang. Never calls a /usr/bin
/// shim such as clang or python3: without the tools, they open Apple's prompt.
func hasCommandLineTools() -> Bool {
    let r = run("/usr/bin/xcode-select", ["-p"])
    let dir = r.output.trimmingCharacters(in: .whitespacesAndNewlines)
    guard r.status == 0, !dir.isEmpty else { return false }
    return ["usr/bin/clang", "Toolchains/XcodeDefault.xctoolchain/usr/bin/clang"].contains {
        FileManager.default.isExecutableFile(atPath: "\(dir)/\($0)")
    }
}

@discardableResult
func run(_ tool: String, _ args: [String]) -> (status: Int32, output: String) {
    let p = Process()
    p.executableURL = URL(fileURLWithPath: tool)
    p.arguments = args
    let pipe = Pipe()
    p.standardOutput = pipe
    p.standardError = FileHandle.nullDevice
    do { try p.run() } catch { return (-1, "") }
    let data = pipe.fileHandleForReading.readDataToEndOfFile()
    p.waitUntilExit()
    return (p.terminationStatus, String(decoding: data, as: UTF8.self))
}

// MARK: - Model

enum Phase: Equatable { case ready, installing, done, failed(String) }

/// What the installer knows about one game.
struct GameState: Equatable {
    var game: URL?
    var linux: URL?
    var selected = false
    var userChose = false      // the user ticked or unticked it: detection no longer changes it
    var built = false
    var dataIssue: String?
    var ready: Bool { game != nil && linux != nil && dataIssue == nil }
}

@MainActor
final class Installer: ObservableObject {
    @Published var tools = false
    @Published var states: [String: GameState] = Dictionary(uniqueKeysWithValues: allGames.map { ($0.id, GameState()) })
    @Published var phase = Phase.ready
    @Published var step = ""
    @Published var progress = 0.0
    @Published var log: [String] = []
    @Published var warnings: [String] = []
    @Published var showLog = false
    private var process: Process?
    private var timer: Timer?
    private var queue: [Game] = []       // games still to build in this run
    private var runCount = 1, runIndex = 0

    func state(_ game: Game) -> GameState { states[game.id] ?? GameState() }
    var chosen: [Game] { allGames.filter { state($0).selected && state($0).ready } }
    var canInstall: Bool { tools && !chosen.isEmpty && phase != .installing }
    var built: [Game] { allGames.filter { state($0).built } }

    func setSelected(_ game: Game, _ on: Bool) {
        states[game.id]?.selected = on
        states[game.id]?.userChose = true
    }

    func setGame(_ game: Game, _ url: URL) { states[game.id]?.game = url; tickIfReady(game) }
    func setLinux(_ game: Game, _ url: URL) { states[game.id]?.linux = url; tickIfReady(game) }

    /// A game becomes ticked when it is found, unless the user unticked it.
    private func tickIfReady(_ game: Game) {
        guard var s = states[game.id] else { return }
        s.dataIssue = s.game.flatMap { game.dataIssue($0) }
        if s.ready && !s.userChose { s.selected = true }
        if !s.ready { s.selected = false }
        states[game.id] = s
    }

    func refresh() {
        Task.detached {
            let t = hasCommandLineTools()
            let found = allGames.map { ($0, $0.findGame(), $0.findLinux()) }
            await MainActor.run {
                self.tools = t
                for (game, g, l) in found {
                    let s = self.state(game)
                    if s.game == nil || !game.isGameFolder(s.game!) || game.dataIssue(s.game!) != nil { self.states[game.id]?.game = g }
                    if s.linux == nil || !game.isLinuxDepot(s.linux!) { self.states[game.id]?.linux = l }
                    self.tickIfReady(game)
                }
            }
        }
    }

    /// Polls while something is missing: the tools installer and Steam downloads finish on their own.
    func startWatching() {
        refresh()
        timer = Timer.scheduledTimer(withTimeInterval: 4, repeats: true) { [weak self] _ in
            Task { @MainActor in
                guard let self, self.phase != .installing else { return }
                if !self.tools || allGames.contains(where: { !self.state($0).ready }) { self.refresh() }
            }
        }
    }

    func installTools() { run("/usr/bin/xcode-select", ["--install"]) }

    func openSteamConsole() { NSWorkspace.shared.open(URL(string: "steam://open/console")!) }

    func copy(_ text: String) {
        NSPasteboard.general.clearContents()
        NSPasteboard.general.setString(text, forType: .string)
    }

    func choose(_ title: String, expected: String, valid: @escaping (URL) -> Bool, set: @escaping (URL) -> Void) {
        let panel = NSOpenPanel()
        panel.message = title
        panel.canChooseDirectories = true
        panel.canChooseFiles = false
        panel.directoryURL = steamSupport
        guard panel.runModal() == .OK, let url = panel.url else { return }
        if valid(url) { set(url) } else {
            let alert = NSAlert()
            alert.messageText = "This is not the right folder"
            alert.informativeText = "Choose the folder that contains \(expected)."
            alert.runModal()
        }
    }

    func install() {
        let games = chosen
        guard !games.isEmpty else { return }
        phase = .installing
        log = []; warnings = []; step = "Unpacking the source"; progress = 0
        for game in allGames { states[game.id]?.built = false }
        queue = games; runCount = games.count; runIndex = 0
        Task.detached {
            do { try unpackSource() } catch {
                await MainActor.run { self.phase = .failed("Could not unpack the source: \(error.localizedDescription)") }
                return
            }
            await MainActor.run { self.runNext() }
        }
    }

    /// setup.sh once per ticked game, with its folders: the same build/ and
    /// config/local.json, each game in its own build tree.
    private func runNext() {
        guard !queue.isEmpty else { phase = .done; return }
        let game = queue.removeFirst()
        let s = state(game)
        guard let gameDir = s.game, let linux = s.linux else { runNext(); return }
        runSetup(game: game, gameDir: gameDir, linux: linux)
    }

    private func runSetup(game: Game, gameDir: URL, linux: URL) {
        let p = Process()
        p.executableURL = URL(fileURLWithPath: "/bin/bash")
        p.arguments = [sourceDir.appendingPathComponent("setup.sh").path, "--game", game.id, gameDir.path, linux.path]
        p.currentDirectoryURL = sourceDir
        var env = ProcessInfo.processInfo.environment
        // A Finder app gets a bare PATH: add Rust's and Homebrew's, used when present.
        let extra = [home.appendingPathComponent(".cargo/bin").path, "/opt/homebrew/bin"]
        env["PATH"] = (extra + ["/usr/bin", "/bin", "/usr/sbin", "/sbin"]).joined(separator: ":")
        p.environment = env
        let pipe = Pipe()
        p.standardOutput = pipe
        p.standardError = pipe
        try? FileManager.default.createDirectory(at: logFile.deletingLastPathComponent(), withIntermediateDirectories: true)
        if runIndex == 0 { FileManager.default.createFile(atPath: logFile.path, contents: nil) }
        let logHandle = try? FileHandle(forWritingTo: logFile)
        _ = try? logHandle?.seekToEnd()
        let buffer = LineBuffer()
        // Everything happens on the handler's serial queue: the last bytes are
        // read and logged before the exit status is looked at (end of file).
        pipe.fileHandleForReading.readabilityHandler = { handle in
            let chunk = handle.availableData
            if !chunk.isEmpty {
                logHandle?.write(chunk)
                let lines = buffer.append(chunk)
                if !lines.isEmpty { Task { @MainActor in self.consume(lines) } }
                return
            }
            handle.readabilityHandler = nil
            try? logHandle?.close()
            let rest = buffer.flush()
            p.waitUntilExit()
            let status = p.terminationStatus
            Task { @MainActor in
                self.consume(rest)
                if status == 0 {
                    self.states[game.id]?.built = true
                    self.runIndex += 1
                    self.runNext()
                } else {
                    self.queue = []
                    let error = self.log.last { $0.hasPrefix("error:") } ?? "setup.sh stopped (exit \(status))."
                    self.phase = .failed("\(game.title): " + error.replacingOccurrences(of: "error: ", with: ""))
                    self.showLog = true
                }
            }
        }
        do { try p.run(); process = p } catch { phase = .failed("Could not start setup.sh: \(error.localizedDescription)") }
    }

    /// Progress from setup.sh's output: its steps, then build_game.py's
    /// "lifted a/b jobs" (10-35 %) and "compiled a/b files" (35-95 %).
    static func progress(after line: String, from current: Double) -> Double {
        func fraction(_ prefix: String) -> Double? {
            guard let r = line.range(of: prefix) else { return nil }
            let parts = line[r.upperBound...].prefix { $0 != " " && $0 != "," }.split(separator: "/")
            guard parts.count == 2, let a = Double(parts[0]), let b = Double(parts[1]), b > 0 else { return nil }
            return min(a / b, 1)
        }
        var value = current
        if line.hasPrefix("==> Recompiling") { value = 0.10 }
        else if line.hasPrefix("==> Creating") { value = 0.95 }
        else if line.hasPrefix("==> ") { value = min(current + 0.012, 0.10) }
        else if let f = fraction("lifted ") { value = 0.10 + 0.25 * f }
        else if let f = fraction("compiled ") { value = 0.35 + 0.60 * f }
        return max(current, value)
    }

    private func consume(_ lines: [String]) {
        for line in lines {
            // Each game's run fills its own share of the bar.
            let share = 1.0 / Double(max(runCount, 1)), base = Double(runIndex) * share
            let inRun = min(max((progress - base) / share, 0), 1)
            progress = base + share * Installer.progress(after: line, from: inRun)
            if line.hasPrefix("==> ") {
                step = String(line.dropFirst(4))
            } else if line.hasPrefix("warning: ") {
                warnings.append(String(line.dropFirst(9)))
            }
            log.append(line)
        }
        if log.count > 4000 { log.removeFirst(log.count - 4000) }
    }

    func cancel() { process?.interrupt() }

    func open(_ game: Game) { NSWorkspace.shared.open(game.installedApp) }
}

/// Splits setup.sh's output into lines. FileHandle calls its readability
/// handler on one serial queue, so the buffer is never touched concurrently.
final class LineBuffer: @unchecked Sendable {
    private var pending = Data()
    func append(_ chunk: Data) -> [String] {
        pending.append(chunk)
        var lines: [String] = []
        while let nl = pending.firstIndex(of: 0x0a) {
            lines.append(String(decoding: pending[pending.startIndex..<nl], as: UTF8.self))
            pending.removeSubrange(pending.startIndex...nl)
        }
        return lines
    }
    /// The unterminated last line, if any.
    func flush() -> [String] {
        defer { pending = Data() }
        return pending.isEmpty ? [] : [String(decoding: pending, as: UTF8.self)]
    }
}

/// Replaces the source in Application Support by the bundled snapshot. build/,
/// deps/ and config/local.json survive, so a new version rebuilds incrementally.
/// The snapshot is unpacked beside the folder first (a failure leaves the old
/// source), and a folder without the installer's marker is never touched.
let ownerMarker = ".portal2-installer"

func installerError(_ message: String) -> NSError {
    NSError(domain: "installer", code: 1, userInfo: [NSLocalizedDescriptionKey: message])
}

nonisolated func unpackSource() throws {
    guard let archive = Bundle.main.url(forResource: "source", withExtension: "tar.gz") else {
        throw installerError("source.tar.gz is missing from the app")
    }
    let fm = FileManager.default
    let marker = sourceDir.appendingPathComponent(ownerMarker)
    if fm.fileExists(atPath: sourceDir.path), !fm.fileExists(atPath: marker.path),
       !(try fm.contentsOfDirectory(atPath: sourceDir.path)).isEmpty {
        throw installerError("\(sourceDir.path) exists and was not made by this installer: move it away and try again")
    }
    let staging = sourceDir.deletingLastPathComponent().appendingPathComponent(".\(sourceDir.lastPathComponent).staging")
    try? fm.removeItem(at: staging)
    try fm.createDirectory(at: staging, withIntermediateDirectories: true)
    let r = run("/usr/bin/tar", ["-xzf", archive.path, "-C", staging.path])
    guard r.status == 0 else {
        try? fm.removeItem(at: staging)
        throw installerError("could not unpack the source (tar exit \(r.status))")
    }
    // tar copies the downloaded app's quarantine onto every file it extracts.
    run("/usr/bin/xattr", ["-dr", "com.apple.quarantine", staging.path])
    try fm.createDirectory(at: sourceDir, withIntermediateDirectories: true)
    fm.createFile(atPath: marker.path, contents: nil)
    // What survives: generated files and the local configuration.
    let keep: Set<String> = ["build", "deps", "config", ownerMarker]
    for name in try fm.contentsOfDirectory(atPath: sourceDir.path) where !keep.contains(name) {
        try fm.removeItem(at: sourceDir.appendingPathComponent(name))
    }
    let config = sourceDir.appendingPathComponent("config")
    if isDir(config) {
        for name in try fm.contentsOfDirectory(atPath: config.path) where name != "local.json" {
            try fm.removeItem(at: config.appendingPathComponent(name))
        }
    }
    for name in try fm.contentsOfDirectory(atPath: staging.path) {
        let from = staging.appendingPathComponent(name), to = sourceDir.appendingPathComponent(name)
        if name == "config", isDir(to) {
            for item in try fm.contentsOfDirectory(atPath: from.path) {
                try fm.moveItem(at: from.appendingPathComponent(item), to: to.appendingPathComponent(item))
            }
        } else {
            try fm.moveItem(at: from, to: to)
        }
    }
    try fm.removeItem(at: staging)
}

// MARK: - Views

struct StepRow<Content: View>: View {
    let number: Int
    let title: String
    let ok: Bool
    @ViewBuilder let content: Content

    var body: some View {
        HStack(alignment: .firstTextBaseline, spacing: 12) {
            ZStack {
                Circle().fill(ok ? Color.green : Color.secondary.opacity(0.18)).frame(width: 22, height: 22)
                if ok { Image(systemName: "checkmark").font(.system(size: 11, weight: .bold)).foregroundStyle(.white) }
                else { Text("\(number)").font(.system(size: 12, weight: .semibold)).foregroundStyle(.secondary) }
            }
            .alignmentGuide(.firstTextBaseline) { $0[VerticalAlignment.center] + 5 }
            VStack(alignment: .leading, spacing: 6) {
                Text(title).font(.headline)
                content.font(.callout).foregroundStyle(.secondary)
            }
            Spacer(minLength: 0)
        }
    }
}

struct Command: View {
    let text: String
    let copy: (String) -> Void
    var body: some View {
        HStack(spacing: 6) {
            Text(text).font(.system(.callout, design: .monospaced)).textSelection(.enabled)
                .padding(.horizontal, 8).padding(.vertical, 3)
                .background(RoundedRectangle(cornerRadius: 5).fill(Color.secondary.opacity(0.12)))
            Button("Copy") { copy(text) }.controlSize(.small)
        }
    }
}

struct PathLine: View {
    let url: URL?
    let missing: String
    var body: some View {
        if let url { Text(url.path).lineLimit(1).truncationMode(.middle).help(url.path) }
        else { Text(missing).fixedSize(horizontal: false, vertical: true) }
    }
}

let windowTitle = "Portal 2 and Half-Life 2 for Apple silicon"

/// One game: a checkbox, then where its files are or how to get them.
struct GameRow: View {
    @ObservedObject var model: Installer
    let game: Game

    var body: some View {
        let s = model.state(game)
        VStack(alignment: .leading, spacing: 6) {
            Toggle(isOn: Binding(get: { s.selected }, set: { model.setSelected(game, $0) })) {
                Text(game.title).font(.headline).foregroundStyle(.primary)
            }
            .toggleStyle(.checkbox)
            .disabled(!s.ready || model.phase == .installing)
            VStack(alignment: .leading, spacing: 6) {
                if let url = s.game {
                    PathLine(url: url, missing: "")
                    if let issue = s.dataIssue {
                        Text(issue).foregroundStyle(.orange).fixedSize(horizontal: false, vertical: true)
                        Command(text: "download_depot \(game.appID) \(game.dataDepot)", copy: model.copy)
                    }
                } else {
                    Text("Game data not found: install \(game.title) with Steam (the current version), choose its folder, or paste this in the Steam console and wait for “Depot download complete”:")
                        .fixedSize(horizontal: false, vertical: true)
                    Command(text: "download_depot \(game.appID) \(game.dataDepot)", copy: model.copy)
                }
                if let url = s.linux {
                    PathLine(url: url, missing: "")
                } else {
                    Text("Linux binaries: paste this in the Steam console, then wait for “Depot download complete” (\(game.linuxSize)):")
                        .fixedSize(horizontal: false, vertical: true)
                    Command(text: game.depotCommand, copy: model.copy)
                }
                HStack {
                    if !s.ready { Button("Open Steam console") { model.openSteamConsole() } }
                    if s.game == nil || s.dataIssue != nil {
                        Button("Choose game folder…") {
                            model.choose("Choose the \(game.title) folder (it contains \(game.marker))", expected: "the \(game.marker) folder",
                                         valid: game.isGameFolder) { model.setGame(game, $0) }
                        }
                    }
                    if s.linux == nil {
                        Button("Choose Linux binaries…") {
                            model.choose("Choose the depot_\(game.linuxDepot) folder of the Linux binaries",
                                         expected: game.linuxFiles[0], valid: game.isLinuxDepot) { model.setLinux(game, $0) }
                        }
                    }
                }
            }
            .padding(.leading, 20)
            .font(.callout).foregroundStyle(.secondary)
        }
    }
}

struct ContentView: View {
    @ObservedObject var model: Installer

    var body: some View {
        VStack(alignment: .leading, spacing: 0) {
            VStack(alignment: .leading, spacing: 4) {
                Text(windowTitle).font(.title2.weight(.semibold))
                Text("Builds native Mac apps from your own copies of the games on Steam. Nothing from the games is downloaded by this installer.")
                    .font(.callout).foregroundStyle(.secondary).fixedSize(horizontal: false, vertical: true)
            }
            .padding(.bottom, 20)

            VStack(alignment: .leading, spacing: 18) {
                StepRow(number: 1, title: "Xcode command line tools", ok: model.tools) {
                    if model.tools { Text("Installed.") } else {
                        Text("Apple's compiler, needed to build the games (about 1 GB).")
                        Button("Install…") { model.installTools() }
                    }
                }
                StepRow(number: 2, title: "Games to build", ok: !model.chosen.isEmpty) {
                    VStack(alignment: .leading, spacing: 14) {
                        ForEach(allGames) { GameRow(model: model, game: $0) }
                        Text("Half-Life 2 is experimental: it plays on Mac, with fewer hours of testing.")
                            .font(.caption)
                    }
                }
                StepRow(number: 3, title: "Build", ok: model.phase == .done) {
                    buildStatus
                }
            }

            Spacer(minLength: 16)
            footer
        }
        .padding(24)
        .frame(width: 600)
        .frame(minHeight: 600)
    }

    @ViewBuilder var buildStatus: some View {
        switch model.phase {
        case .ready:
            let games = model.chosen
            let minutes = games.map(\.minutes).reduce(0, +)
            Text(games.isEmpty ? "Tick at least one game found above."
                 : "About \(minutes) minutes the first time and \(5 * games.count) GB of disk. Re-running later only rebuilds what changed.")
        case .installing:
            ProgressView(value: model.progress)
            Text(model.step).lineLimit(1)
        case .done:
            Text("\(model.built.map { "“\($0.appName)”" }.joined(separator: " and ")) \(model.built.count > 1 ? "are" : "is") in your Applications folder. Open Steam with the account that owns \(model.built.count > 1 ? "the games, then open an app" : "the game, then open the app").")
                .fixedSize(horizontal: false, vertical: true)
            ForEach(model.warnings, id: \.self) { Text("⚠︎ " + $0).foregroundStyle(.orange) }
        case .failed(let message):
            Text(message).foregroundStyle(.red).fixedSize(horizontal: false, vertical: true)
        }
    }

    var footer: some View {
        VStack(alignment: .leading, spacing: 10) {
            if model.showLog {
                ScrollViewReader { proxy in
                    ScrollView {
                        Text(model.log.suffix(400).joined(separator: "\n"))
                            .font(.system(size: 11, design: .monospaced)).textSelection(.enabled)
                            .frame(maxWidth: .infinity, alignment: .leading).id("end")
                    }
                    .frame(height: 140)
                    .background(RoundedRectangle(cornerRadius: 6).fill(Color.secondary.opacity(0.08)))
                    .onChange(of: model.log.count) { _ in proxy.scrollTo("end", anchor: .bottom) }
                }
            }
            HStack {
                Button(model.showLog ? "Hide details" : "Show details") { model.showLog.toggle() }
                    .buttonStyle(.link)
                Spacer()
                switch model.phase {
                case .installing:
                    Button("Stop") { model.cancel() }
                case .done:
                    Button("Rebuild") { model.install() }
                    ForEach(model.built) { game in
                        Button("Open \(game.title)") { model.open(game) }
                    }
                default:
                    Button(model.phase == .ready ? "Build" : "Try again") { model.install() }
                        .keyboardShortcut(.defaultAction).disabled(!model.canInstall)
                }
            }
        }
    }
}

// MARK: - App

final class AppDelegate: NSObject, NSApplicationDelegate {
    func applicationShouldTerminateAfterLastWindowClosed(_ sender: NSApplication) -> Bool { true }
}

struct InstallerApp: App {
    @NSApplicationDelegateAdaptor(AppDelegate.self) var delegate
    @StateObject private var model = Installer()
    var body: some Scene {
        WindowGroup(windowTitle) {
            ContentView(model: model).onAppear { model.startWatching() }
        }
        .windowResizability(.contentSize)
    }
}

@MainActor
func snapshot(to path: String, state: String) {
    let model = Installer()
    model.tools = state != "empty"
    if state != "empty" {
        model.states["portal2"] = GameState(game: steamSupport.appendingPathComponent("steamapps/common/Portal 2"),
                                            linux: state == "missing" ? nil : URL(fileURLWithPath: "/depot_661"), selected: state != "missing")
        model.states["hl2"] = GameState(game: steamSupport.appendingPathComponent("steamapps/common/Half-Life 2"),
                                        linux: state == "missing" ? nil : URL(fileURLWithPath: "/depot_224"), selected: state != "missing")
    }
    switch state {
    case "incomplete":
        model.states["portal2"]?.dataIssue = "Incomplete game data: portal2_dlc2/scripts/editoritems.txt is missing, unreadable or empty. An empty placeholder does not help. In Steam, verify the integrity of Portal 2's files (Properties > Installed Files), choose a complete game folder, or download the game data below:"
        model.states["portal2"]?.selected = false
    case "building": model.phase = .installing
        model.step = "Recompiling Portal 2 (about 10 minutes the first time, seconds afterwards)"; model.progress = 0.25
        model.showLog = true; model.log = ["==> Preparing the Linux modules of Portal 2", "[lift] engine.so: 41 233 functions", "[cc] 312/1024"]
    case "done": model.phase = .done
        for id in ["portal2", "hl2"] { model.states[id]?.built = true }
    case "failed": model.phase = .failed("Half-Life 2: lib/libSDL2.dylib not found")
    default: break
    }
    let renderer = ImageRenderer(content: ContentView(model: model).background(Color(nsColor: .windowBackgroundColor)))
    renderer.scale = 2
    guard let image = renderer.nsImage, let tiff = image.tiffRepresentation,
          let png = NSBitmapImageRep(data: tiff)?.representation(using: .png, properties: [:]) else {
        print("snapshot failed"); exit(1)
    }
    try? png.write(to: URL(fileURLWithPath: path))
}

@main
enum Main {
    static func main() {
        let args = CommandLine.arguments
        if let i = args.firstIndex(of: "--snapshot"), i + 1 < args.count {
            MainActor.assumeIsolated { snapshot(to: args[i + 1], state: i + 2 < args.count ? args[i + 2] : "ready") }
            return
        }
        if args.contains("--self-test") {
            var p = 0.0
            for line in ["==> Checking tools", "==> Recompiling Portal 2", "  lifted 40/80 jobs: 1 functions", "  compiled 33/330 files, 12 s",
                         "==> Recompiling again", "==> Creating ~/Applications/Portal 2 Natif.app"] {
                p = Installer.progress(after: line, from: p)
                print(String(format: "%.3f", p), line)
            }
            return
        }
        if args.contains("--unpack") {   // tests: unpack into sourceDir and exit
            do { try unpackSource(); print("unpacked into", sourceDir.path) } catch { print("error:", error.localizedDescription); exit(1) }
            return
        }
        if args.contains("--detect") {
            print("command line tools:", hasCommandLineTools())
            for game in allGames {
                let g = game.findGame(), l = game.findLinux()
                let issue = g.flatMap { game.dataIssue($0) }
                print("\(game.id): game \(g?.path ?? "-"), linux \(l?.path ?? "-")\(g != nil && l != nil && issue == nil ? " (ticked)" : "")")
                if let issue { print(issue) }
            }
            return
        }
        InstallerApp.main()
    }
}
