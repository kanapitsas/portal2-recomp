// Portal 2 Recomp installer: a window around ./setup.sh for people who do not
// use Terminal. It carries a snapshot of the repository (Resources/source.tar.gz,
// made by tools/build_installer.sh), unpacks it in Application Support, finds
// the game the way setup.sh does, then runs setup.sh and shows its steps.
// It never downloads the game: the Steam console does, by the user's hand.
//
//   installer --snapshot out.png [state]   renders the window off screen
//   installer --detect                     prints what it finds, then exits
import AppKit
import SwiftUI

let appName = "Portal 2 Natif"
// $HOME first (the same folder for a Finder app), so tests can point elsewhere.
let home = ProcessInfo.processInfo.environment["HOME"].map { URL(fileURLWithPath: $0) }
    ?? FileManager.default.homeDirectoryForCurrentUser
let support = home.appendingPathComponent("Library/Application Support/\(appName)")
// Source and build (~5 GB), apart from the saves in `support`: uninstall.sh
// trashes it with the app.
let sourceDir = home.appendingPathComponent("Library/Application Support/\(appName) Build")
let steamSupport = home.appendingPathComponent("Library/Application Support/Steam")
let installedApp = home.appendingPathComponent("Applications/\(appName).app")
let logFile = home.appendingPathComponent("Library/Logs/Portal2Installer.log")

// MARK: - Detection (same rules as setup.sh)

func isDir(_ url: URL) -> Bool {
    var dir: ObjCBool = false
    return FileManager.default.fileExists(atPath: url.path, isDirectory: &dir) && dir.boolValue
}

func isGameFolder(_ url: URL) -> Bool { isDir(url.appendingPathComponent("portal2")) }

func isLinuxDepot(_ url: URL) -> Bool {
    FileManager.default.fileExists(atPath: url.appendingPathComponent("bin/linux32/engine.so").path)
        && FileManager.default.fileExists(atPath: url.appendingPathComponent("portal2/bin/linux32/server.so").path)
}

/// Folder written by the Steam console's download_depot (setup.sh's find_depot).
func findDepot(_ id: Int) -> URL? {
    guard isDir(steamSupport) else { return nil }
    let out = run("/usr/bin/find", [steamSupport.path, "-maxdepth", "9", "-type", "d",
                                    "-path", "*/content/app_620/depot_\(id)"]).output
    return out.split(separator: "\n").first.map { URL(fileURLWithPath: String($0)) }
}

func findGame() -> URL? {
    let installed = steamSupport.appendingPathComponent("steamapps/common/Portal 2")
    if isGameFolder(installed) { return installed }
    if let depot = findDepot(621), isGameFolder(depot) { return depot }
    return nil
}

func findLinux() -> URL? {
    if let depot = findDepot(661), isLinuxDepot(depot) { return depot }
    return nil
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

@MainActor
final class Installer: ObservableObject {
    @Published var tools = false
    @Published var game: URL?
    @Published var linux: URL?
    @Published var phase = Phase.ready
    @Published var step = ""
    @Published var progress = 0.0
    @Published var log: [String] = []
    @Published var warnings: [String] = []
    @Published var showLog = false
    private var process: Process?
    private var timer: Timer?

    var canInstall: Bool { tools && game != nil && linux != nil && phase != .installing }

    func refresh() {
        Task.detached {
            let t = hasCommandLineTools(), g = findGame(), l = findLinux()
            await MainActor.run {
                self.tools = t
                if self.game == nil || !isGameFolder(self.game!) { self.game = g }
                if self.linux == nil || !isLinuxDepot(self.linux!) { self.linux = l }
            }
        }
    }

    /// Polls while something is missing: the tools installer and Steam downloads finish on their own.
    func startWatching() {
        refresh()
        timer = Timer.scheduledTimer(withTimeInterval: 4, repeats: true) { [weak self] _ in
            Task { @MainActor in
                guard let self, self.phase != .installing else { return }
                if !self.tools || self.game == nil || self.linux == nil { self.refresh() }
            }
        }
    }

    func installTools() { run("/usr/bin/xcode-select", ["--install"]) }

    func openSteamConsole() { NSWorkspace.shared.open(URL(string: "steam://open/console")!) }

    func copy(_ text: String) {
        NSPasteboard.general.clearContents()
        NSPasteboard.general.setString(text, forType: .string)
    }

    func choose(_ title: String, valid: @escaping (URL) -> Bool, set: @escaping (URL) -> Void) {
        let panel = NSOpenPanel()
        panel.message = title
        panel.canChooseDirectories = true
        panel.canChooseFiles = false
        panel.directoryURL = steamSupport
        guard panel.runModal() == .OK, let url = panel.url else { return }
        if valid(url) { set(url) } else {
            let alert = NSAlert()
            alert.messageText = "This is not the right folder"
            alert.informativeText = "Choose the folder that contains \(title.contains("Linux") ? "bin/linux32/engine.so" : "the portal2 folder")."
            alert.runModal()
        }
    }

    func install() {
        guard let game, let linux else { return }
        phase = .installing
        log = []; warnings = []; step = "Unpacking the source"; progress = 0
        Task.detached {
            do { try unpackSource() } catch {
                await MainActor.run { self.phase = .failed("Could not unpack the source: \(error.localizedDescription)") }
                return
            }
            await MainActor.run { self.runSetup(game: game, linux: linux) }
        }
    }

    private func runSetup(game: URL, linux: URL) {
        let p = Process()
        p.executableURL = URL(fileURLWithPath: "/bin/bash")
        p.arguments = [sourceDir.appendingPathComponent("setup.sh").path, game.path, linux.path]
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
        FileManager.default.createFile(atPath: logFile.path, contents: nil)
        let logHandle = try? FileHandle(forWritingTo: logFile)
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
                    self.phase = .done
                } else {
                    let error = self.log.last { $0.hasPrefix("error:") } ?? "setup.sh stopped (exit \(status))."
                    self.phase = .failed(error.replacingOccurrences(of: "error: ", with: ""))
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
            progress = Installer.progress(after: line, from: progress)
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

    func openGame() { NSWorkspace.shared.open(installedApp) }
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

struct ContentView: View {
    @ObservedObject var model: Installer

    var body: some View {
        VStack(alignment: .leading, spacing: 0) {
            VStack(alignment: .leading, spacing: 4) {
                Text("Portal 2 for Apple silicon").font(.title2.weight(.semibold))
                Text("Builds a native Mac app from your own copy of Portal 2 on Steam. Nothing from the game is downloaded by this installer.")
                    .font(.callout).foregroundStyle(.secondary).fixedSize(horizontal: false, vertical: true)
            }
            .padding(.bottom, 20)

            VStack(alignment: .leading, spacing: 18) {
                StepRow(number: 1, title: "Xcode command line tools", ok: model.tools) {
                    if model.tools { Text("Installed.") } else {
                        Text("Apple's compiler, needed to build the game (about 1 GB).")
                        Button("Install…") { model.installTools() }
                    }
                }
                StepRow(number: 2, title: "Portal 2 game files", ok: model.game != nil) {
                    PathLine(url: model.game, missing: "Install Portal 2 in Steam, or download the Linux game data in the Steam console (12.6 GB):")
                    if model.game == nil {
                        Command(text: "download_depot 620 621", copy: model.copy)
                    }
                    HStack {
                        if model.game == nil { Button("Open Steam console") { model.openSteamConsole() } }
                        Button("Choose folder…") {
                            model.choose("Choose the Portal 2 folder (it contains portal2)", valid: isGameFolder) { model.game = $0 }
                        }
                    }
                }
                StepRow(number: 3, title: "Linux binaries", ok: model.linux != nil) {
                    PathLine(url: model.linux, missing: "Paste this in the Steam console, then wait for “Depot download complete” (about 90 MB):")
                    if model.linux == nil {
                        Command(text: "download_depot 620 661", copy: model.copy)
                    }
                    HStack {
                        if model.linux == nil { Button("Open Steam console") { model.openSteamConsole() } }
                        Button("Choose folder…") {
                            model.choose("Choose the depot_661 folder of the Linux binaries", valid: isLinuxDepot) { model.linux = $0 }
                        }
                    }
                }
                StepRow(number: 4, title: "Build the game", ok: model.phase == .done) {
                    buildStatus
                }
            }

            Spacer(minLength: 16)
            footer
        }
        .padding(24)
        .frame(width: 560)
        .frame(minHeight: 520)
    }

    @ViewBuilder var buildStatus: some View {
        switch model.phase {
        case .ready:
            Text("About 10 minutes the first time and 5 GB of disk. Re-running later only rebuilds what changed.")
        case .installing:
            ProgressView(value: model.progress)
            Text(model.step).lineLimit(1)
        case .done:
            Text("“\(appName)” is in your Applications folder. Open Steam with the account that owns Portal 2, then open it.")
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
                    Button("Open Portal 2") { model.openGame() }.keyboardShortcut(.defaultAction)
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
        WindowGroup("Portal 2 Installer") {
            ContentView(model: model).onAppear { model.startWatching() }
        }
        .windowResizability(.contentSize)
    }
}

@MainActor
func snapshot(to path: String, state: String) {
    let model = Installer()
    model.tools = state != "empty"
    if state != "empty" { model.game = steamSupport.appendingPathComponent("steamapps/common/Portal 2") }
    switch state {
    case "building": model.linux = URL(fileURLWithPath: "/depot_661"); model.phase = .installing
        model.step = "Recompiling the game (about 10 minutes the first time, seconds afterwards)"; model.progress = 0.5
        model.showLog = true; model.log = ["==> Preparing the Linux modules", "[lift] engine.so: 41 233 functions", "[cc] 312/1024"]
    case "done": model.linux = URL(fileURLWithPath: "/depot_661"); model.phase = .done
    case "failed": model.linux = URL(fileURLWithPath: "/depot_661"); model.phase = .failed("lib/libSDL2.dylib not found")
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
            for line in ["==> Checking tools", "==> Recompiling the game", "  lifted 40/80 jobs: 1 functions", "  compiled 33/330 files, 12 s",
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
            print("game:", findGame()?.path ?? "-")
            print("linux:", findLinux()?.path ?? "-")
            return
        }
        InstallerApp.main()
    }
}
