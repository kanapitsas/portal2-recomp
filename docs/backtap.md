# Back taps: detector, recordings and dataset

Portal 2 on iPhone ([ios.md](ios.md#back-taps)). A candidate opens at half the
thresholds: |user acceleration| and its sample-to-sample change (jerk), or a
sudden change in rotation around x; it is judged 100 ms later against the full
thresholds. Two paths: acceleration (0.10 g, 6 g/s) or gyroscope (0.06 g,
6 g/s, 0.30 rad/s rotation, phone still just before); on the left, the index finger rotates the phone
(0.4 to 0.7 rad/s) while barely accelerating it (0.07 to 0.10 g).
Side: score = 0.5 × brief rotation around the device's x axis (vertical in
landscape, relative to the screen orientation, or to gravity as a fallback) + rotation
around z relative to the screen (the tapping hand pivots the phone within its
own plane: to the left, z > 0, for a left tap, in both landscape
orientations); 79/79 labeled taps, versus 72/79 with x alone (taps near a
corner, phone flipped); a
"lateral" fallback (tap on the edge of the phone) uses the sign of the y acceleration.
Rejections: menu open or overlay hidden (up to 80 ms before the peak), a touch starting or ending within ±80 ms,
agitation or a change in gravity before the peak (a 300 ms window, looking back as far as 600 ms, skipping the 250 ms following the two previous candidates; in the 50 ms just before, their vibration must have dropped below 25% of the peak, otherwise it counts as a jolt), phone lying flat screen-up (gravity z < -0.95). Click:
press, release after 50 ms and at least two frames. Defaults
calibrated on 2026-10-26 on an iPhone 15 Pro Max (`P2_BACKTAP_LOG=1`): 30 out of 30
back taps on the correct side in both landscape orientations; the hand makes the
phone vibrate 50 to 100 ms after the tap, hence the relaxed duration limit;
CoreMotion delivers 100 Hz at most. Taps on the edge of the phone have not been
tried. Hard taps peak 30 to 40 ms after they start and make the
phone vibrate for more than 150 ms: rise-time criterion dropped, vibration excluded for
250 ms (measured: 0.03 to 0.04 g RMS beyond that). Known limitation: the first jolt of a sudden movement from rest
looks like a tap (filtering it out would delay every shot).

The motion sensors run only while a tap can be accepted: device motion
updates stop after 2 s in a menu or with the overlay hidden for a controller
or keyboard, at once when the app becomes inactive, and restart as soon as the game is back
(log: `[ios] tapes au dos : capteurs arrêtés` / `en marche`). The first 150 ms
after a restart count as menu time, so the touch that closed the menu (handled
by SDL, not the overlay) cannot fire a portal. Recordings and the dataset
therefore have gaps in menus; dataset labels still pending when the sensors
stop are written when they restart.

Raw recording for tuning the thresholds on the Mac:
Settings > Portal 2 > Record taps (one dated file per launch in `Documents/backtap/`,
visible in the Files app), or `P2_BACKTAP_RAW=Documents/backtap.raw` (samples and touches), then
`cc -O2 -o build/backtap-replay tools/backtap_replay.c -lm` and
`build/backtap-replay backtap.raw` (same `P2_BACKTAP_*` variables). `P2_TOUCH_CONTROLS=0` also disables back taps.

## Tap dataset (guessed labels)

While Settings > Portal 2 > Record taps is on (or with `P2_BACKTAP_DATASET`
set), the game also keeps a dataset of back-tap candidates, labeled from what
the player does next, as groundwork for a detector that learns from each player
(`runtime/ios/backtap_dataset.c`, tested by `tests/checks_backtap_dataset.c`).
A second detector with a wider gate (a quarter of the thresholds instead of
half; `P2_BACKTAP_WIDE`) runs next to the game's, which is unchanged, so taps
too soft to reach the game's detector are recorded too. Each candidate becomes
one CSV line in `Documents/backtap/dataset.csv`, kept across sessions: the
detector's features, the raw sensor window (0.35 s before the peak to the
verdict, 100 ms after), and a label set 1.5 s later:

| Label | Rule |
|---|---|
| `fired` | the game fired within 60 ms of it |
| `wrong_side` | fired, then the other portal 0.2 to 1.2 s later with the camera still (under 60 mouse counts, ~4°): probably the wrong side, the label side is the other one |
| `missed` | not fired, at least half the game's thresholds (acceleration or x rotation), and a shot follows 0.2 to 1.0 s later: probably a missed tap tried again; one per shot, the strongest |
| `after_fire` | not fired, less than 1.5 s after a shot (hand rebound) |
| `quiet` | none of the above: probably not a tap |

The ✗ button (top right, left of pause; only while the dataset is on, so never
with Record taps off) lets
the player say "that was wrong". Its meaning comes from context, and it takes
precedence over the guessed labels:

| Label | Rule |
|---|---|
| `user_false` | a shot less than 2.5 s before the press, with no other shot in between, and the other portal does not follow within 2 s: false detection |
| `user_wrong_side` | same, but the other portal follows within 2 s of the press: wrong side, the label side is the other one |
| `user_missed` | no shot in the 2.5 s before the press: the strongest candidate of that window (not a rebound, not within 100 ms of a touch, since pressing the button shakes the phone) was a missed tap, on the side of the shot that follows within 2 s if any |

Labels are therefore written 4.8 s after the candidate (2.5 s + 2 s, plus the
game detector's verdict delay). With several presses, the first one that
applies counts.

These labels are hints, not ground truth: the columns `prev_dt`, `next_dt`,
`next_side` and `next_look` keep the timing and camera motion to the
neighbouring shots, so labels can be recomputed with other rules offline.
Replaying the 2026-10-26 recordings (25 minutes, mostly calibration) gives
about 2.5 candidates per second, 13 shots a minute, 0.6 `wrong_side` and 5
`missed` a minute; the file grows by about 30 MB per hour of play.

The FPS counter shows a third line, `DATA taps/corrections Xreported`:
examples labeled as taps (`fired`, `missed`, `wrong_side`, `user_wrong_side`,
`user_missed`), corrections (`missed`, `wrong_side` and every `user_*`), and
those reported with the ✗ button, counted over the whole file. The line turns
yellow for a second when ✗ is pressed; the X count goes up about 5 s later, and
not at all if no candidate matched (a missed tap too soft even for the wide
detector). Raw recordings (`P2_BACKTAP_RAW`) also log presses (`f` lines),
which the replay tool reads.

| Variable | Default | Purpose |
|---|---|---|
| `P2_BACKTAP_DATASET` | on with Record taps (`P2_BACKTAP_RAW`), off otherwise | `1` turns it on in `Documents/backtap/dataset.csv`, a path (relative to HOME) turns it on there, `0` keeps it off even with Record taps |
| `P2_BACKTAP_TAG` | empty | written on every line, to mark a session with known intent (e.g. `left-only`, `no-taps`) |
| `P2_BACKTAP_WIDE` | 0.25 | fraction of the thresholds that opens a candidate of the wide detector |

`tools/ios_device.sh backtap` copies the file to
`build/ios/device/backtap/dataset.csv`; `P2_BACKTAP_DATASET=<file.csv>
build/backtap-replay <recording>` builds the same file from a raw recording
(without camera motion, the `wrong_side` rule ignores the camera).

| Variable | Default | Purpose |
|---|---|---|
| `P2_BACKTAP_ACCEL` | 0.10 g | minimum acceleration peak |
| `P2_BACKTAP_JERK` | 6 g/s | minimum jerk |
| `P2_BACKTAP_ROT` | 0.08 rad/s | minimum rotation score to decide the side |
| `P2_BACKTAP_SIDE_X` | 0.5 | weight of rotation around x relative to rotation around z (`100`: x only, the old rule) |
| `P2_BACKTAP_GYRO_ACCEL` | 0.06 g | gyroscope path: minimum acceleration peak |
| `P2_BACKTAP_GYRO_ROT` | 0.30 rad/s | gyroscope path: minimum rotation around x (`99` disables it) |
| `P2_BACKTAP_GYRO_JERK` | 15 rad/s² | gyroscope path: rotation change that opens a candidate |
| `P2_BACKTAP_GYRO_CALM` | 0.03 g | gyroscope path: maximum agitation in the preceding 50 ms (phone still) |
| `P2_BACKTAP_REFRACTORY` | 0.25 s | nothing after a candidate |
| `P2_BACKTAP_TOUCH_GUARD` | 0.08 s | nothing around the start or end of a touch |
| `P2_BACKTAP_FLAT` | -0.95 g | gravity z below this threshold: phone lying flat, ignored (`-2`: never) |
| `P2_BACKTAP_WIDTH` | 0.12 s | maximum peak duration (above 35% of its maximum; measured over 100 ms, so inactive by default) |
| `P2_BACKTAP_AGITATION` | 0.05 g | maximum RMS over the preceding 300 ms (excluding vibration from taps) |
| `P2_BACKTAP_GRAVITY` | 0.12 g | maximum change in gravity over the preceding 200 ms |
| `P2_BACKTAP_INVERT` | 0 | `1`: left and right swapped |
| `P2_BACKTAP_LATERAL` | 1 | tap on the edge of the phone: `-1` reversed direction, `0` ignored |
| `P2_BACKTAP_ZSIGN` | 0 | `±1`: requires this sign of a_z at the start of the peak (back vs. screen) |
| `P2_BACKTAP_HOLD` | 0.05 s | press duration |
| `P2_BACKTAP_HZ` | 200 | rate requested from CoreMotion (100 Hz obtained on iPhone 15 Pro Max) |

Log: `[ios] tape <s> : pic, (x y z), secousse, rotation x y z, montée (délai du maximum),
durée, avant (agitation), gravité (variation, puis vecteur), toucher (écart en
ms), orientation -> côté ou raison du rejet` (the log is in French: tap, peak, (x y z), jerk,
rotation x y z, rise (time to maximum), duration, before (agitation), gravity (change, then
vector), touch (gap in ms), orientation -> side or rejection reason). Findings: a back tap has
a_z < 0 at the peak, a tap on the screen a_z > 0.

