# Ardour DAW — Comprehensive Codebase Explainer

> **Purpose:** A full in-depth reference explaining every major module, file, and
> system in the Ardour codebase.  Special attention is given to (a) the existing
> Event-API / interaction-logging infrastructure that can be extended for AI
> workflows, and (b) the UI theming and styling system.

---

## Table of Contents

1. [Repository Layout](#1-repository-layout)
2. [Core Libraries (`libs/`)](#2-core-libraries-libs)
   - 2.1 [ardour – Main DAW Engine](#21-ardour--main-daw-engine)
   - 2.2 [pbd – Persistent Data Base](#22-pbd--persistent-data-base)
   - 2.3 [evoral – Musical Event Management](#23-evoral--musical-event-management)
   - 2.4 [temporal – Time & Tempo System](#24-temporal--time--tempo-system)
   - 2.5 [gtkmm2ext – GTK2/C++ Extensions](#25-gtkmm2ext--gtk2c-extensions)
   - 2.6 [canvas – Cairo Drawing System](#26-canvas--cairo-drawing-system)
   - 2.7 [widgets – Advanced UI Components](#27-widgets--advanced-ui-components)
   - 2.8 [midi++2 – MIDI I/O](#28-midi2--midi-io)
   - 2.9 [backends – Audio Backend Abstraction](#29-backends--audio-backend-abstraction)
   - 2.10 [surfaces – Control Surface Protocols](#210-surfaces--control-surface-protocols)
   - 2.11 [ctrl-interface – Control Protocol Framework](#211-ctrl-interface--control-protocol-framework)
   - 2.12 [audiographer – Audio Analysis & Export](#212-audiographer--audio-analysis--export)
   - 2.13 [panners – Spatial Audio](#213-panners--spatial-audio)
   - 2.14 [Other Libraries](#214-other-libraries)
3. [GTK2 User Interface (`gtk2_ardour/`)](#3-gtk2-user-interface-gtk2_ardour)
   - 3.1 [Main Application Framework](#31-main-application-framework)
   - 3.2 [Editor (Timeline / DAW View)](#32-editor-timeline--daw-view)
   - 3.3 [Mixer (Mixing Console)](#33-mixer-mixing-console)
   - 3.4 [Plugin / Processor UI](#34-plugin--processor-ui)
   - 3.5 [MIDI Editing](#35-midi-editing)
   - 3.6 [Transport & Time Display](#36-transport--time-display)
   - 3.7 [Dialogs & Windows](#37-dialogs--windows)
   - 3.8 [Analysis & Visualization](#38-analysis--visualization)
   - 3.9 [Automation & Curves](#39-automation--curves)
   - 3.10 [Trigger / Clip System](#310-trigger--clip-system)
4. [UI Theming & Styling System](#4-ui-theming--styling-system)
   - 4.1 [GTK RC Theme Files](#41-gtk-rc-theme-files)
   - 4.2 [Color Theme Files (`.colors`)](#42-color-theme-files-colors)
   - 4.3 [Color Alias System](#43-color-alias-system)
   - 4.4 [Dynamic Theme Manager](#44-dynamic-theme-manager)
   - 4.5 [UI Config Variables](#45-ui-config-variables)
   - 4.6 [Canvas Styling](#46-canvas-styling)
5. [Event API & Interaction Logging](#5-event-api--interaction-logging)
   - 5.1 [SessionEvent — Real-Time Transport Events](#51-sessionevent--real-time-transport-events)
   - 5.2 [PBD Signal / Slot System](#52-pbd-signal--slot-system)
   - 5.3 [PBD EventLoop Framework](#53-pbd-eventloop-framework)
   - 5.4 [PBD Error / Warning Logging](#54-pbd-error--warning-logging)
   - 5.5 [Control Protocol Signals](#55-control-protocol-signals)
   - 5.6 [Lua Editor Hook Scripts](#56-lua-editor-hook-scripts)
   - 5.7 [MIDI Tracer](#57-midi-tracer)
   - 5.8 [DSP Statistics](#58-dsp-statistics)
   - 5.9 [Pingback (Opt-In Reporting)](#59-pingback-opt-in-reporting)
6. [AI Workflow Integration — Where to Add a User-Interaction Event API](#6-ai-workflow-integration--where-to-add-a-user-interaction-event-api)
   - 6.1 [Recommended Hook Points](#61-recommended-hook-points)
   - 6.2 [Suggested Architecture: `UserInteractionLogger`](#62-suggested-architecture-userinteractionlogger)
   - 6.3 [Connecting the Logger to Existing Signals](#63-connecting-the-logger-to-existing-signals)
7. [Session Management](#7-session-management)
8. [Audio Engine & Backends](#8-audio-engine--backends)
9. [Scripting & Automation API (Lua)](#9-scripting--automation-api-lua)
10. [Control Surfaces (`libs/surfaces/`)](#10-control-surfaces-libssurfaces)
11. [Build System & Configuration](#11-build-system--configuration)
12. [Shared Resources (`share/`)](#12-shared-resources-share)
13. [Quick Reference Table](#13-quick-reference-table)

---

## 1. Repository Layout

```
ardour/
├── gtk2_ardour/        # GTK2-based graphical user interface (~400 files, ~245 K LOC)
├── libs/               # Core C++ libraries (~37 subdirectories)
│   ├── ardour/         # Main DAW engine (libardour)
│   ├── pbd/            # Persistent Data Base – infrastructure utilities
│   ├── evoral/         # MIDI / musical event data model
│   ├── temporal/       # Time, tempo, BBT system
│   ├── gtkmm2ext/      # GTK2 C++ widget extensions
│   ├── canvas/         # Cairo scene-graph renderer
│   ├── widgets/        # Custom GTK widgets (faders, knobs, meters …)
│   ├── midi++2/        # MIDI I/O ports and MMC
│   ├── backends/       # Pluggable audio I/O (JACK, ALSA, CoreAudio, PortAudio …)
│   ├── surfaces/       # Hardware control surface drivers (MCU, OSC, Push2 …)
│   ├── ctrl-interface/ # Control protocol abstraction (BasicUI, ControlProtocol)
│   ├── audiographer/   # Audio analysis, export pipeline
│   ├── panners/        # Stereo / surround panning algorithms
│   ├── lua/            # Embedded Lua 5.3 scripting engine
│   ├── fluidsynth/     # FluidSynth GM synthesiser
│   ├── vst3/           # Steinberg VST3 SDK
│   ├── qm-dsp/         # Spectral / MIR algorithms
│   ├── vamp-plugins/   # VAMP plugin host
│   ├── vamp-pyin/      # Pitch-detection plugin
│   ├── libltc/         # LTC timecode encoder / decoder
│   ├── zita-convolver/ # High-quality convolution reverb
│   ├── zita-resampler/ # High-quality sample-rate conversion
│   └── …               # (plus pluginUI helpers, hidapi, etc.)
├── luasession/         # Headless Lua-driven session scripting
├── headless/           # Non-GUI Ardour binary support
├── session_utils/      # CLI session utility programs
├── share/              # Installed runtime resources
│   ├── scripts/        # Bundled Lua scripts (50+ examples)
│   ├── templates/      # Session templates
│   ├── midi_maps/      # MIDI controller mapping files
│   ├── patchfiles/     # Plugin preset collections
│   ├── osc/            # OSC surface definitions
│   └── web_surfaces/   # Web-based control surface HTML/JS
├── doc/                # Architecture diagrams and documentation
├── tools/              # Build and developer tools
├── waf / wscript       # WAF (Python) build system
└── COPYING             # GPL v2 licence
```

---

## 2. Core Libraries (`libs/`)

### 2.1 `ardour` — Main DAW Engine

**Library name:** `libardour`  
**Path:** `libs/ardour/`

The central library that models every aspect of an audio production session.

| File / Header | Purpose |
|---|---|
| `ardour/session.h` | God-object: owns all tracks, regions, playlists, locations, automation, history, transport state, and the process graph |
| `session.cc` / `session_state.cc` / `session_events.cc` / `session_rtevents.cc` | Implementation split across ~12 files (>25 000 LOC total) |
| `ardour/route.h` | A single signal path (track or bus): processors, automation, metering |
| `ardour/track.h` | Route subclass with disk I/O (audio or MIDI track) |
| `ardour/audioengine.h` | Wraps the chosen audio backend; owns the real-time process thread |
| `ardour/session_event.h` | Asynchronous event queue fed from UI thread, consumed in RT thread (see §5.1) |
| `ardour/processor.h` | Abstract base for anything that sits in a signal chain (amp, EQ, plugin, meter …) |
| `ardour/plugin.h` | Plugin abstraction (VST2/3, LV2, AU, LADSPA) |
| `ardour/automation_control.h` | A single automatable parameter; carries an `AutomationList` |
| `ardour/automatable.h` | Mixin that adds parameter automation to any object |
| `ardour/region.h` / `audioregion.h` / `midiregion.h` | Immutable content referenced from playlists |
| `ardour/playlist.h` | Ordered, non-overlapping sequence of regions for one track |
| `ardour/location.h` | Named positions: markers, loop range, punch range, cue markers |
| `ardour/triggerbox.h` | Clip-launch / trigger system (Ableton-style clip grid) |
| `ardour/midi_model.h` | Note/CC data model for MIDI regions |
| `ardour/port.h` / `audio_port.h` / `midi_port.h` | Physical I/O port wrappers |
| `ardour/export_handler.h` | Session-level audio export orchestration |
| `ardour/luascripting.h` | Embedded Lua engine bindings |

**Key design patterns:**

- The `Session` class emits dozens of **PBD signals** (e.g., `RouteAdded`, `RegionAdded`, `TransportStateChange`) that allow any subscriber — UI, Lua scripts, control surfaces, or a future AI logger — to react to state changes.
- A **real-time event queue** (`SessionEvent`) lets the UI thread schedule transport commands (locate, loop, punch) that are executed safely inside the RT callback.
- **Undo/redo** is managed through `pbd/history_owner.h`; every editing operation is wrapped in a `Command` object.

---

### 2.2 `pbd` — Persistent Data Base

**Library name:** `libpbd`  
**Path:** `libs/pbd/`

The infrastructure layer used by every other Ardour library.

| File | Purpose |
|---|---|
| `pbd/signals.h` | Type-safe signal/slot (observer) mechanism — the backbone of all inter-module communication |
| `pbd/event_loop.h` | Abstract event-loop interface; enables cross-thread slot calls |
| `pbd/base_ui.h` | Concrete event loop + thread pair (subclassed by `ARDOUR_UI`, surface drivers, etc.) |
| `pbd/controllable.h` | A named, IPC-exposed parameter (e.g. fader position, plugin knob) |
| `pbd/command.h` | Base class for undo/redo commands |
| `pbd/history_owner.h` | Undo/redo stack manager |
| `pbd/xml++.h` | XML read/write (session files are pure XML) |
| `pbd/id.h` | Unique 64-bit object IDs |
| `pbd/error.h` | Logging macros: `error`, `warning`, `info`, `fatal` — write to `PBD::warning` / `PBD::error` streams |
| `pbd/debug.h` | Compile- and runtime-selectable debug channels (bitmask) |
| `pbd/ringbuffer.h` | Lock-free single-producer / single-consumer ring buffer |
| `pbd/pool.h` | Per-thread memory pool (used by `SessionEvent` to avoid RT allocation) |
| `pbd/file_utils.h` | Filesystem helpers |
| `pbd/downloader.h` | Async HTTP download (used for plugin and update downloads) |

**Signals are the primary IPC mechanism.** A simplified example:

```cpp
// Emitting
Session::RouteAdded (route_list);      // emitted by Session

// Subscribing (anywhere in the codebase)
_session->RouteAdded.connect (
    *this, MISSING_INVALIDATOR,
    boost::bind (&MyClass::on_route_added, this, _1),
    gui_context()                       // ensures callback runs on the GUI thread
);
```

---

### 2.3 `evoral` — Musical Event Management

**Library name:** `libevoral`  
**Path:** `libs/evoral/`

Provides the data model for MIDI and other musical events inside regions.

| File | Purpose |
|---|---|
| `evoral/Event.h` | A single timed event (type + data bytes + timestamp) |
| `evoral/Note.h` | A note-on / note-off pair with pitch, velocity, channel, duration |
| `evoral/Sequence.h` | Ordered, queryable list of Notes + other events |
| `evoral/ControlList.h` | Interpolated automation curve (shared with `ardour/automation_list`) |
| `evoral/ControlSet.h` | A set of `ControlList` objects (automation on one object) |
| `evoral/SMF.h` | Standard MIDI File read/write |
| `evoral/Curve.h` | Mathematical interpolation curves |

---

### 2.4 `temporal` — Time & Tempo System

**Library name:** `libtemporal`  
**Path:** `libs/temporal/`

All time arithmetic in Ardour goes through this library.

| Concept | Details |
|---|---|
| **Timepos / Timecnt** | Unified time point and duration; can hold samples or musical beats |
| **BBT_Time** | Bar/Beat/Tick position |
| **TempoMap** | Piecewise tempo and time-signature map; handles tempo ramps |
| **Timecode** | SMPTE / EBU timecode (25, 29.97, 30 fps etc.) |
| **TimeRange** | A start–end time interval |

This is a recent (Ardour 7+) redesign that removed hard dependencies on the audio sample rate from musical-time reasoning.

---

### 2.5 `gtkmm2ext` — GTK2/C++ Extensions

**Library name:** `libgtkmm2ext`  
**Path:** `libs/gtkmm2ext/`

Thin layer of GTK2 helpers and custom widgets that the main UI consumes.

| File | Purpose |
|---|---|
| `gtkmm2ext/gtk_ui.h` | Main application object (wraps `Gtk::Main`); owns the GUI event loop |
| `gtkmm2ext/bindings.h` | Key and mouse binding registry |
| `gtkmm2ext/cairocell.h` | Cairo-rendered cell renderers for `Gtk::TreeView` |
| `gtkmm2ext/visibility_tracker.h` | Tracks widget show/hide state |
| `gtkmm2ext/utils.h` | Miscellaneous GTK helpers |

---

### 2.6 `canvas` — Cairo Drawing System

**Library name:** `libardourcanvas`  
**Path:** `libs/canvas/`

A scene-graph renderer built on Cairo, used for the timeline, the MIDI piano roll, and automation curves.

| Component | Details |
|---|---|
| **`Canvas`** | Root object; owns redraw scheduling and Cairo surface |
| **`Item`** | Base class for every drawable object (Line, Rectangle, Text, WaveView, etc.) |
| **`GtkCanvas`** | GTK widget that hosts a `Canvas` |
| **`ScrollGroup`** | A group whose coordinate system scrolls independently |
| **`WaveView`** | Waveform rendering item (audio track display) |
| **`CanvasNoteEvent`** | A MIDI note rectangle in the piano roll |
| **Event handling** | Mouse/keyboard events are dispatched through the item tree; each `Item` can have a `sigc::signal` slot for event callbacks |

---

### 2.7 `widgets` — Advanced UI Components

**Library name:** `libardourwidgets`  
**Path:** `libs/widgets/`

Reusable custom widgets shared across `gtk2_ardour` and control surface UIs.

| Widget | Description |
|---|---|
| `ArdourButton` | Themed toggle/push button |
| `ArdourFader` | Linear or log-scale fader |
| `ArdourKnob` | Rotary control with mouse gesture mapping |
| `FastMeter` | High-refresh-rate VU / peak meter |
| `BarController` | Compact bar-style parameter display |
| `SearchBar` | Incremental-search text entry |
| `Tabbable` | Dock-able / detachable tabbed panel |
| `TearOff` | Detachable toolbar section |
| `Popup` | Floating tooltip/popup widget |

---

### 2.8 `midi++2` — MIDI I/O

**Library name:** `libmidi++`  
**Path:** `libs/midi++2/`

Low-level MIDI port management and protocol helpers.

| Component | Details |
|---|---|
| `midi++/port.h` | Abstract MIDI port (input or output) |
| `midi++/mmc.h` | MIDI Machine Control: transport and device control |
| `midi++/parser.h` | Byte-stream to event parser |
| `midi++/manager.h` | Port lifecycle (create, destroy, enumerate) |

---

### 2.9 `backends` — Audio Backend Abstraction

**Library name:** dynamically loaded plugins  
**Path:** `libs/backends/`

Each subdirectory is a separate shared library that Ardour loads at runtime.

| Backend | OS | Notes |
|---|---|---|
| `jack/` | Linux, macOS, Windows | JACK Audio Connection Kit |
| `alsa/` | Linux | Direct ALSA sequencer / PCM |
| `coreaudio/` | macOS | CoreAudio API |
| `portaudio/` | Windows, macOS | PortAudio layer |
| `pulseaudio/` | Linux | PulseAudio |
| `dummy/` | All | Offline / headless processing |

Every backend implements `ARDOUR::AudioBackend` which declares:
- Device enumeration and configuration
- Sample rate / buffer size negotiation
- Port creation and connection
- Latency reporting
- Transport master / slave modes

---

### 2.10 `surfaces` — Control Surface Protocols

**Library name:** dynamically loaded plugins  
**Path:** `libs/surfaces/`

Each subdirectory is a plugin providing one hardware protocol.

| Protocol | Directory | Description |
|---|---|---|
| Mackie MCU / HUI | `mackie/` | 8-channel motorized fader banks; most professional mixers |
| Behringer X-Touch | *(MCU variant)* | Extended MCU with extra display capabilities |
| PreSonus FaderPort 1 | `faderport/` | Single touch-sensitive fader |
| PreSonus FaderPort 8/16 | `faderport8/` | Multi-fader versions |
| Novation Launchpad Pro | `launchpad_pro/` | RGB pad grid controller |
| Novation Launchpad X | `launchpad_x/` | Consumer pad grid |
| Novation Launchkey 4 | `launchkey_4/` | Keyboard with pads and encoders |
| Ableton Push 2 | `push2/` | 64-pad grid with touch strip and high-res display |
| NI Maschine 2 | `maschine2/` | Complex pad controller with browser display |
| OSC | `osc/` | Open Sound Control over UDP/TCP – ideal for mobile apps |
| Generic MIDI | `generic_midi/` | Learn-mode MIDI controller mapping |
| Contour Design | `contourdesign/` | Shuttle Pro and similar devices |
| Console1 | `console1/` | Softube Console 1 channel strip |
| CC121 | `cc121/` | Yamaha CC121 channel strip |
| US-2400 | `us2400/` | Tascam US-2400 control surface |
| Websockets | `websockets/` | Browser-based control via WebSocket |
| Wiimote | `wiimote/` | Nintendo Wii Remote |

---

### 2.11 `ctrl-interface` — Control Protocol Framework

**Library name:** `libcontrolcp`  
**Path:** `libs/ctrl-interface/`

Defines the abstract interface that every control surface plugin must implement.

#### `ControlProtocol` (`control_protocol/control_protocol.h`)

```
ARDOUR::Session           (pointer)
│
├── name()                    protocol identifier string
├── active()                  enable / disable
├── set_active()
│
├── stripable_selection_changed()   notify surface of selection changes
├── route_table                     virtual fader → route mapping
│
└── Static signals (callable from any surface, handled in gtk2_ardour):
        ZoomToSession, ZoomIn, ZoomOut
        Undo, Redo
        ScrollTimeline
        GotoView (n)
        StepTracksUp / StepTracksDown
        PluginSelected (plugin)
```

#### `BasicUI` (`control_protocol/basic_ui.h`)

High-level transport and mixing actions available to every surface without
direct `Session` access:

```
Transport:   play(), stop(), record_enable_toggle(), loop_toggle()
             set_transport_speed(), rewind(), ffwd()
Navigation:  goto_start(), goto_end(), next_marker(), prev_marker()
             locate (sample)
Editing:     add_marker(), remove_marker_at_playhead()
             undo(), redo()
Monitoring:  monitor_mute(), monitor_dim(), monitor_mono()
Triggers:    cue_start (row), stop_all_cues(), stop_cue (col)
Scenes:      store_mixer_scene (n), recall_mixer_scene (n)
```

#### `MIDISurface` (`midi_surface/midi_surface.h`)

Base class for all MIDI-based surfaces.  Handles:
- MIDI port lifecycle
- Incoming MIDI parsing → surface-specific virtual methods
- Outgoing MIDI feedback (LEDs, faders, display)
- SysEx message construction

---

### 2.12 `audiographer` — Audio Analysis & Export

**Library name:** `libaudiographer`  
**Path:** `libs/audiographer/`

A pipeline-based audio processing framework used during export.

| Component | Details |
|---|---|
| `Process context` | Type-safe chunk of audio samples flowing through the graph |
| `Sink` / `Source` | Interfaces for nodes in the pipeline |
| `Chunker` | Splits or joins buffer boundaries |
| `SampleRateConverter` | Wraps `libzita-resampler` |
| `Normalizer` | Two-pass loudness normalisation |
| `EBUR128Analyser` | EBU R128 integrated loudness measurement |
| `SndfileWriter` | WAV/AIFF export via libsndfile |
| `FlacWriter` | FLAC export |
| `OggVorbisWriter` | Ogg Vorbis export |
| `LoudnessReader` | Real-time loudness monitoring |

---

### 2.13 `panners` — Spatial Audio

**Library name:** dynamically loaded plugins  
**Path:** `libs/panners/`

| Panner | Details |
|---|---|
| `stereopan/` | Mono-to-stereo (balance) and stereo-to-stereo panning |
| `2in2out/` | Independent L/R gain panner |
| `1in2out/` | Standard pan law with configurable curve |
| `vbap/` | Vector-Based Amplitude Panning (N channels) |
| `wdl/` | Surround panning using WDL library |

---

### 2.14 Other Libraries

| Library | Purpose |
|---|---|
| `lua/` | Embedded Lua 5.3 interpreter |
| `fluidsynth/` | FluidSynth GM soft-synth |
| `vst3/` | Steinberg VST3 plug-in SDK |
| `qm-dsp/` | Queen Mary DSP: spectral, onset, beat tracking |
| `vamp-plugins/` | VAMP analysis plug-in host |
| `vamp-pyin/` | Probabilistic YIN pitch estimator |
| `libltc/` | Linear Timecode (LTC) encode/decode |
| `zita-convolver/` | Fast partitioned convolution |
| `zita-resampler/` | Zero-latency sample rate conversion |
| `hidapi/` | Cross-platform HID device access (for some surfaces) |
| `tk/` | Token-based resource allocation |
| `ptformat/` | Pro Tools session format reader |
| `aaf/` | AAF (Advanced Authoring Format) reader |
| `staffpad/` | StaffPad notation engine integration |

---

## 3. GTK2 User Interface (`gtk2_ardour/`)

The graphical front-end contains approximately **400 source files and 245 000 lines of
C++**.  It is built on GTK 2 (via `gtkmm2ext`) with custom Cairo rendering
(`canvas`).

### 3.1 Main Application Framework

| File | Description |
|---|---|
| `ardour_ui.h` / `ardour_ui.cc` | Top-level `ARDOUR_UI` class; owns the main window, menu bar, and GTK main loop |
| `ardour_ui2.cc` | Session-open/close logic, startup wizard |
| `ardour_ui3.cc` | Status bar, toolbar widgets |
| `ardour_ui_dependents.cc` | Constructs all major sub-windows once a session is open |
| `ardour_ui_dialogs.cc` | Lazy-loads dialog objects |
| `ardour_ui_mixer.cc` | Wires the `MixerUI` into the application |
| `ardour_ui_session.cc` | Handles session events (saves, state changes) on the GUI thread |
| `ardour_ui_ed.cc` | Wires the `Editor` into the application |
| `actions.cc` | Registers all `Gtk::Action` objects (menu items, toolbar buttons) |
| `keyboard.cc` | Global keyboard event handler |
| `keyeditor.h/cc` | UI for editing key bindings |
| `application_bar.h/cc` | Application-level transport and session info bar |

---

### 3.2 Editor (Timeline / DAW View)

The timeline editor is the largest single subsystem.

| File | Description |
|---|---|
| `editor.h` / `editor.cc` | Main `Editor` class; ~9 000 LOC |
| `editor_ops.cc` | Editing operations: cut, copy, paste, align, nudge, stretch |
| `editor_drag.cc` | Mouse drag handlers for moving / trimming regions |
| `editor_canvas.cc` | Canvas setup and event routing |
| `editor_regions.h/cc` | Region browser panel |
| `editor_routes.h/cc` | Track/bus list panel |
| `editor_markers.cc` | Location marker creation and editing |
| `editor_selection.cc` | Selection management (tracks, regions, time ranges) |
| `editor_rulers.cc` | Timeline ruler and time-signature display |
| `editor_cursors.h/cc` | Playhead, edit cursor, select/range cursor sprites |
| `editor_automation_line.h/cc` | Automation curve overlay on timeline |
| `editor_snapshots.cc` | Snapshot browser |
| `editor_timefx.cc` | Time-stretch / pitch-shift dialog |
| `editor_group_tabs.h/cc` | Track group colour tabs |
| `editor_summary.h/cc` | Thumbnail "navigator" view of the whole session |
| `selection.h/cc` | `Selection` data structure (which regions, tracks, time ranges are selected) |
| `streamview.h/cc` | Per-track canvas layer that hosts region views |
| `region_view.h/cc` | Base class for all drawn regions |
| `audio_region_view.h/cc` | Waveform display for audio regions |
| `time_axis_view.h/cc` | A single horizontal track row in the editor |
| `ghostregion.h/cc` | Translucent drag preview |

---

### 3.3 Mixer (Mixing Console)

| File | Description |
|---|---|
| `mixer_ui.h/cc` | Main `Mixer_UI` class (~4 700 LOC); grid of strips |
| `mixer_strip.h/cc` | A single track / bus strip: fader, panners, metering, inserts |
| `meter_strip.h/cc` | Metering-only strip for the level overview panel |
| `foldback_strip.h/cc` | Cue / artist-mix strip |
| `mixer_group_tabs.h/cc` | Colour-coded group tabs |
| `monitor_section.h/cc` | Dedicated control room monitor section |

---

### 3.4 Plugin / Processor UI

| File | Description |
|---|---|
| `processor_box.h/cc` | Horizontal chain of plugin / processor tiles |
| `plugin_ui.h/cc` | Wraps any plugin GUI; manages plugin window lifecycle |
| `lv2_plugin_ui.h/cc` | Embeds an LV2 UI |
| `vst_plugin_ui.cc` | Hosts a VST2 GUI window |
| `au_pluginui.mm` | Hosts an Audio Unit GUI (macOS) |
| `generic_pluginui.cc` | Auto-generated fader/knob UI for any plugin |
| `plugin_pin_dialog.h/cc` | Drag-and-drop I/O pin routing for plugins |
| `plugin_selector.h/cc` | Plugin browser and installer |

---

### 3.5 MIDI Editing

| File | Description |
|---|---|
| `midi_view.h/cc` | Base MIDI-in-region canvas view (~5 600 LOC) |
| `pianoroll.h/cc` | Standalone piano-roll editor (~2 300 LOC) |
| `pianoroll_window.h/cc` | Detachable window wrapper for the piano roll |
| `midi_region_view.h/cc` | Inline piano roll inside the timeline |
| `midi_list_editor.h/cc` | List-based note / CC editor |
| `midi_tracer.h/cc` | Live MIDI event monitor (shows incoming bytes) |
| `midi_channel_dialog.h/cc` | Channel selector |

---

### 3.6 Transport & Time Display

| File | Description |
|---|---|
| `main_clock.h/cc` | Primary position display (session bar) |
| `audio_clock.h/cc` | Editable clock widget (~2 400 LOC); supports all time formats |
| `transport_control.h/cc` | Play/stop/record/loop button widget |
| `big_transport_window.h/cc` | Expanded detachable transport panel |
| `big_clock_window.h/cc` | Fullscreen clock for external displays |
| `tempo_dialog.h/cc` | Tempo / time-signature editing dialog |

---

### 3.7 Dialogs & Windows

| File | Description |
|---|---|
| `session_dialog.h/cc` | New / open session wizard |
| `engine_dialog.h/cc` | Audio and MIDI device configuration (~3 400 LOC) |
| `export_dialog.h/cc` | Full export workflow UI |
| `option_editor.h/cc` | Preferences / settings panel |
| `ardour_color_dialog.h/cc` | Color picker with RGBA hex entry |
| `add_route_dialog.h/cc` | "Add tracks and buses" dialog |
| `new_user_wizard.h/cc` | First-launch setup wizard |
| `loudness_dialog.h/cc` | Integrated loudness analysis and normalization |
| `video_server_dialog.h/cc` | ArdourVideo server connection |
| `about.h/cc` | About dialog (version, credits) |

---

### 3.8 Analysis & Visualization

| File | Description |
|---|---|
| `analysis_window.h/cc` | Audio region spectrum display |
| `fft_graph.h/cc` | Cairo-drawn FFT graph |
| `rta_window.h/cc` | Real-time spectrum analyser window |
| `dsp_stats_ui.h/cc` | Per-plugin DSP CPU load bars |
| `dsp_stats_window.h/cc` | Detachable DSP statistics window |

---

### 3.9 Automation & Curves

| File | Description |
|---|---|
| `automation_line.h/cc` | Cairo-drawn automation curve with editable control points |
| `automation_time_axis.h/cc` | Automation lane below a track in the editor |
| `automation_controller.h/cc` | Binds a `Controllable` to a widget for real-time feedback |

---

### 3.10 Trigger / Clip System

| File | Description |
|---|---|
| `trigger_page.h/cc` | Clip-launch grid page (Ableton Session View equivalent) |
| `trigger_strip.h/cc` | Per-track column with clip slots |
| `triggerbox_ui.h/cc` | The 8×N clip matrix widget |
| `clip_editor.h/cc` | Individual clip editor window |
| `cue_editor.h/cc` | Cue marker editor |

---

## 4. UI Theming & Styling System

Ardour's visual appearance is controlled at four separate levels, each
overriding the previous.

### 4.1 GTK RC Theme Files

**Location:** `gtk2_ardour/clearlooks.rc.in`, `gtk2_ardour/ui_light.rc.in`, `gtk2_ardour/ardour2_ui_dark_sae.rc.in`, `gtk2_ardour/ardour2_ui_light_sae.rc.in`

These are **GTK2 RC template files** processed at build time.  They control:

- Standard GTK widget geometry: paddings, border widths, focus indicators
- Font size classes (`small_text`, `medium_text`, `large_text`)
- Per-widget style overrides (`GtkButton`, `GtkEntry`, `GtkTreeView`, `GtkScrollbar`, etc.)
- Color variable references using GTK RC `@variable` syntax

At build time the `@COLOR@` placeholders are expanded.  The result is loaded
by GTK2 when Ardour starts, setting the baseline look of all standard GTK widgets.

```
style "default" = "medium_text" {
  GtkTreeView::even-row-color = @bases
  GtkButton::default-border   = { 0, 0, 0, 0 }
  …
}
```

### 4.2 Color Theme Files (`.colors`)

**Location:** `gtk2_ardour/themes/*.colors`  
**Format:** XML

Each `.colors` file encodes a complete color palette for the application. A
selection of bundled themes:

| File | Style |
|---|---|
| `dark-ardour.colors` | Default dark professional theme |
| `light-ardour.colors` | Light theme |
| `cubasish-ardour.colors` | Cubase-inspired color set |
| `adwaita_dark-ardour.colors` | GNOME Adwaita dark |
| `arc-ardour.colors` | Arc GTK theme adaptation |
| `caineville-ardour.colors` | High-contrast dark |
| `blueberry_milk-ardour.colors` | Soft pastel |
| `clear_gray-ardour.colors` | Flat gray |
| `captain_light-ardour.colors` | Professional light |
| `onedark-ardour.colors` | VS Code One Dark adaptation |
| `recbox-ardour.colors`, `unastudia-ardour.colors`, `xcolors-ardour.colors`, `diehard3-ardour.colors` | Community-contributed themes |

A `.colors` file contains named color entries.  Example fragment:

```xml
<Colors>
  <Color name="theme:bg" value="0x1a1a1aff"/>
  <Color name="theme:fg" value="0xe0e0e0ff"/>
  <Color name="meterstrip:bg" value="0x222222ff"/>
  …
</Colors>
```

### 4.3 Color Alias System

**Files:** `gtk2_ardour/color_aliases.h`

This header is included multiple times (X-macro style) to generate member
variables, setter methods, and getter methods for every named color alias.  It
acts as the "schema" of the color system — all names that can appear in a
`.colors` file must have an alias defined here.

Color categories defined:

- `theme:*` — background, foreground, selection, contrasting surfaces
- `widget:*` — button states, fader track, knob arc
- `meterstrip:*` — level meter colors (10 VU gradient levels)
- `midi:*` — piano-roll note velocities, quantize grid colors
- `alert:*` — red / orange / yellow / green / cyan / blue alert levels
- `transport:*` — play, stop, record button states
- `automation:*` — automation curve and region tinting

### 4.4 Dynamic Theme Manager

**Files:** `gtk2_ardour/color_theme_manager.h`, `gtk2_ardour/color_theme_manager.cc`

`ColorThemeManager` is a singleton that:

1. Reads the user's selected theme name from `UIConfiguration`
2. Loads and parses the corresponding `.colors` XML file
3. Populates the `UIConfiguration` color table
4. Emits `ColorsChanged` signal so all canvas items and custom widgets
   redraw with the new colors
5. Handles user edits from the color picker dialog
   (`ardour_color_dialog.cc`)

This enables **hot-swap theming without restarting** Ardour.

### 4.5 UI Config Variables

**Files:** `gtk2_ardour/ui_config.h`, `gtk2_ardour/ui_config.cc`, `gtk2_ardour/ui_config_vars.inc.h`, `gtk2_ardour/default_ui_config.in`

`UIConfiguration` holds every user-adjustable visual preference:

- Active color theme name
- Font sizes (small / medium / large scale)
- Icon set (built-in vs. custom)
- Waveform display mode (filled, outline, rectified, logarithmic)
- Peak display mode
- Meter type (VU, PPM, K-system)
- Editor grid density
- Scrollbar visibility
- Show/hide various toolbar items

These are stored in `~/.config/ardour8/ui_config` and reloaded at startup.

### 4.6 Canvas Styling

Custom-drawn widgets (waveforms, automation curves, MIDI notes, meters) do
**not** use GTK styles.  Instead each `canvas::Item` reads colors directly from
`UIConfiguration` when it renders.

For example, `WaveView::render()` calls `UIConfiguration::instance().color
("waveform:fg")` at paint time, which means a theme change is reflected
immediately without widget recreation.

---

## 5. Event API & Interaction Logging

Ardour already contains a layered event infrastructure.  This section
catalogues every existing hook point relevant to logging user actions and
session state changes, making it the starting point for any AI integration.

### 5.1 `SessionEvent` — Real-Time Transport Events

**File:** `libs/ardour/ardour/session_event.h`

`SessionEvent` is an **asynchronous command queue** between the UI thread and
the real-time audio thread.

```cpp
struct SessionEvent {
  enum Type {
    SetTransportSpeed,      // play / pause / shuttle
    SetDefaultPlaySpeed,
    Locate,                 // jump to sample position
    LocateRoll,             // jump and start playing
    LocateRollLocate,       // jump, play, jump back (cue audition)
    SetLoop,                // change loop range
    PunchIn,                // start recording at a cue point
    PunchOut,               // stop recording at a cue point
    RangeStop,              // stop at end of play range
    RangeLocate,            // jump to start of range
    Overwrite,              // disk buffer overwrite notification
    OverwriteAll,
    Audition,               // preview a region
    SetPlayAudioRange,      // set which range to play
    CancelPlayAudioRange,
    RealTimeOperation,      // generic RT lambda
    AdjustPlaybackBuffering,
    AdjustCaptureBuffering,
    SetTimecodeTransmission,
    Skip,                   // skip over a muted range
    SetTransportMaster,     // change clock source
    StartRoll,
    EndRoll,
    TransportStateChange,
    SyncCues,
    AutoLoop,               // seamless loop transition
  };
  …
};
```

Every time the user presses Play, jumps to a marker, or changes transport speed
a `SessionEvent` of the appropriate type is queued.

**Logging hook:** Because all transport commands pass through this queue, a single
intercept point in `Session::process_event()` (in `session_events.cc`) can
capture every transport-level user action with sample-accurate timestamps.

### 5.2 PBD Signal / Slot System

**File:** `libs/pbd/pbd/signals.h`

Every significant state change in Ardour is broadcast via a PBD signal.  The
signals emitted by `Session` (declared in `ardour/session.h`) include:

| Signal | Fires when … |
|---|---|
| `RouteAdded` | A track or bus is created |
| `RouteRemoved` | A track or bus is deleted |
| `RegionAdded` | A region is created in any playlist |
| `RegionRemoved` | A region is removed |
| `TransportStateChange` | Play / stop / record state changes |
| `PositionChanged` | Playhead position changes |
| `Located` | A locate (jump) completes |
| `RecordStateChanged` | Record-armed state changes |
| `SoloChanged` | A track is soloed or un-soloed |
| `MuteChanged` | A track is muted or un-muted |
| `TempoChanged` | The tempo map is edited |
| `SessionSaved` | Session is saved to disk |
| `SessionLoaded` | A session is fully loaded |
| `ClipLaunched` | A clip/trigger fires (Ardour 7+) |
| `CueChanged` | A cue marker changes |
| `BundleAdded / BundleRemoved` | I/O connections change |
| `IOChanged` | A track's I/O routing changes |
| `FeedbackDetected` | A routing feedback loop is detected |

Each `Route` also emits per-track signals:
`GainChanged`, `RecordEnableChanged`, `SoloChanged`, `MuteChanged`,
`ProcessorAdded`, `ProcessorRemoved`, `PhaseInversionChanged`, etc.

**Logging hook:** Any observer can connect to any of these signals with a
single `signal.connect(…)` call.  A thin `UserInteractionLogger` class
(see §6) can subscribe to all of them and record timestamped events.

### 5.3 PBD EventLoop Framework

**File:** `libs/pbd/pbd/event_loop.h`

`PBD::EventLoop` is an abstract thread + queue manager.  It provides
`call_slot()`, which allows cross-thread callbacks without explicit locking,
and `InvalidationRecord` for safe lifetime management of callbacks.

The GUI thread's event loop is `ARDOUR_UI` (which inherits from
`Gtkmm2ext::UI`, which inherits from `PBD::BaseUI`).  This means any code
can schedule work to run on the GUI thread using:

```cpp
Glib::signal_idle().connect_once (sigc::mem_fun (*this, &MyClass::on_idle));
// or via PBD:
gui_context()->call_slot (MISSING_INVALIDATOR,
    boost::bind (&MyLogger::record_event, _logger, event));
```

### 5.4 PBD Error / Warning Logging

**File:** `libs/pbd/pbd/error.h`

Four severity levels write to registered output streams:

```cpp
PBD::fatal   << "Critical error message\n";
PBD::error   << "Non-fatal error message\n";
PBD::warning << "Warning message\n";
PBD::info    << "Informational message\n";
```

These can be redirected to any `std::ostream`, a file, a UI console, or a
custom logging back-end.  The GUI console (`gtk2_ardour/ardour_message.cc`)
subscribes to them.

**Logging hook:** Register a custom `ostream`-compatible receiver on
`PBD::info` and `PBD::warning` to capture Ardour's own diagnostic messages.

### 5.5 Control Protocol Signals

**File:** `libs/ctrl-interface/control_protocol/control_protocol/control_protocol.h`

`ControlProtocol` emits static signals that fire when a hardware controller
triggers a UI action:

```cpp
static PBD::Signal<void()> ZoomToSession;
static PBD::Signal<void()> ZoomIn;
static PBD::Signal<void()> ZoomOut;
static PBD::Signal<void()> Undo;
static PBD::Signal<void()> Redo;
static PBD::Signal<void(float)> ScrollTimeline;
static PBD::Signal<void(uint32_t)> GotoView;
static PBD::Signal<void()> StepTracksUp;
static PBD::Signal<void()> StepTracksDown;
static PBD::Signal<void(boost::weak_ptr<ARDOUR::PluginInsert>)> PluginSelected;
```

These are the entry points for user actions originating from physical hardware
rather than from the GUI.

### 5.6 Lua Editor Hook Scripts

**Type:** `LuaScriptInfo::EditorHook`  
**Path:** `share/scripts/`

Lua scripts of type `EditorHook` are notified of editor events in real time.
They receive callbacks for:

- Region selection changes
- Track visibility changes
- Transport start / stop
- Playhead moves

This is already a user-facing **event subscription API** in Lua, accessible
without recompiling Ardour.

**AI workflow hook:** An `EditorHook` script can log interactions to a local
file, a socket, or an HTTP endpoint.  No C++ changes are needed for a
first prototype.

### 5.7 MIDI Tracer

**File:** `gtk2_ardour/midi_tracer.h`, `gtk2_ardour/midi_tracer.cc`

A real-time MIDI event monitor that displays incoming and outgoing MIDI bytes.
It subscribes to MIDI port activity via the `midi++2` `Parser` and renders
events in a scrolling text view.

**AI workflow hook:** The same `Parser::any` signal used by the tracer can feed
an ML model with live MIDI performance data.

### 5.8 DSP Statistics

**Files:** `gtk2_ardour/dsp_stats_ui.h/cc`, `gtk2_ardour/dsp_stats_window.h/cc`

`DSP_Stats` exposes per-plugin CPU measurements obtained from the real-time
thread.  The data is surfaced via `PBD::Signal` updates and rendered as bar
graphs.

**AI workflow hook:** These signals can drive adaptive routing or processing
decisions in an AI assistant.

### 5.9 Pingback (Opt-In Reporting)

**Files:** `gtk2_ardour/pingback.h/cc`

An **optional, user-initiated** anonymized version check that sends the Ardour
version string to `community.ardour.org` using libcurl.  This is not
telemetry; it only fires if the user explicitly opts in during setup.

---

## 6. AI Workflow Integration — Where to Add a User-Interaction Event API

### 6.1 Recommended Hook Points

The table below lists the best existing intercept points, roughly ordered from
highest signal-to-noise to lowest:

| Layer | Hook Location | What It Captures |
|---|---|---|
| **Session signals** | `Session::TransportStateChange`, `Session::Located`, `Session::RegionAdded`, `Session::RouteAdded` | Major session-level user actions |
| **SessionEvent queue** | `Session::process_event()` in `session_events.cc` | Every transport command with exact sample timestamp |
| **Route signals** | `Route::GainChanged`, `Route::MuteChanged`, `Route::SoloChanged`, `Route::ProcessorAdded` | Per-track mixing actions |
| **ControlProtocol signals** | `ControlProtocol::ZoomIn`, `Undo`, `GotoView`, etc. | Physical controller interactions |
| **Lua EditorHook** | `LuaScriptInfo::EditorHook` script | Editor interactions via Lua (no recompile needed) |
| **PBD::info stream** | Register a custom receiver | Diagnostic log entries |
| **MIDI Parser** | `midi++/parser.h` `Parser::any` signal | Every MIDI byte from every device |
| **Automation** | `AutomationControl::Changed` | Parameter automation writes |
| **Undo history** | `Session::UndoHistoryChanged` | All undoable operations |
| **Canvas item events** | `canvas::Item::Event` | Mouse clicks, drags on waveforms/notes |

### 6.2 Suggested Architecture: `UserInteractionLogger`

A minimal new module that intercepts all the above hooks can be added as a
**new file pair** with no breaking changes to existing code:

**Proposed file:** `libs/ardour/ardour/user_interaction_logger.h`

```cpp
// libs/ardour/ardour/user_interaction_logger.h
//
// A non-intrusive observer that records user interactions for AI workflows.
// Connect it to a Session and it will subscribe to all relevant signals.

#pragma once
#include <functional>
#include <string>
#include "pbd/signals.h"
#include "ardour/types.h"

namespace ARDOUR {

class Session;

struct InteractionEvent {
    int64_t     wall_clock_ms;   // glib_get_monotonic_time() / 1000
    samplepos_t session_position; // transport position at time of event
    std::string category;         // "transport", "edit", "mix", "midi", …
    std::string action;           // e.g. "play", "stop", "region_move"
    std::string detail;           // JSON-encoded extra data
};

/** Callback type: receives an interaction event. */
using InteractionCallback = std::function<void(const InteractionEvent&)>;

/**
 * UserInteractionLogger
 *
 * Subscribe this object to a Session and it will invoke the registered
 * callback whenever the user performs a meaningful action.
 *
 * Usage:
 *   auto logger = std::make_unique<UserInteractionLogger>(session);
 *   logger->set_callback([](const InteractionEvent& e){
 *       // write e to file / socket / AI back-end
 *   });
 */
class UserInteractionLogger {
public:
    explicit UserInteractionLogger (Session& s);
    ~UserInteractionLogger ();

    void set_callback (InteractionCallback cb) { _callback = cb; }

private:
    Session&           _session;
    InteractionCallback _callback;

    // PBD signal connections (automatically disconnected on destruction)
    PBD::ScopedConnectionList _connections;

    void on_transport_state_change ();
    void on_located ();
    void on_region_added (std::weak_ptr<Region>);
    void on_route_added (RouteList);
    void on_record_state_changed ();

    void emit (const std::string& category,
               const std::string& action,
               const std::string& detail = "");
};

} // namespace ARDOUR
```

**Proposed file:** `libs/ardour/user_interaction_logger.cc`

```cpp
// libs/ardour/user_interaction_logger.cc
#include "ardour/user_interaction_logger.h"
#include "ardour/session.h"
#include "pbd/glib_event_source.h"   // for g_get_monotonic_time
#include <glib.h>

using namespace ARDOUR;

UserInteractionLogger::UserInteractionLogger (Session& s)
    : _session (s)
{
    s.TransportStateChange.connect (_connections, MISSING_INVALIDATOR,
        boost::bind (&UserInteractionLogger::on_transport_state_change, this),
        boost::bind (&UserInteractionLogger::emit, this, "transport", "state_change", ""));

    s.Located.connect (_connections, MISSING_INVALIDATOR,
        boost::bind (&UserInteractionLogger::on_located, this),
        boost::bind (&UserInteractionLogger::emit, this, "transport", "locate", ""));

    s.RegionAdded.connect (_connections, MISSING_INVALIDATOR,
        boost::bind (&UserInteractionLogger::on_region_added, this, _1),
        boost::bind (&UserInteractionLogger::emit, this, "edit", "region_added", ""));

    s.RouteAdded.connect (_connections, MISSING_INVALIDATOR,
        boost::bind (&UserInteractionLogger::on_route_added, this, _1),
        boost::bind (&UserInteractionLogger::emit, this, "mix", "route_added", ""));

    s.RecordStateChanged.connect (_connections, MISSING_INVALIDATOR,
        boost::bind (&UserInteractionLogger::on_record_state_changed, this),
        boost::bind (&UserInteractionLogger::emit, this, "transport", "record_state", ""));
}

UserInteractionLogger::~UserInteractionLogger () {}

void
UserInteractionLogger::emit (const std::string& category,
                              const std::string& action,
                              const std::string& detail)
{
    if (!_callback) return;
    InteractionEvent e;
    e.wall_clock_ms    = g_get_monotonic_time () / 1000;
    e.session_position = _session.transport_sample ();
    e.category         = category;
    e.action           = action;
    e.detail           = detail;
    _callback (e);
}

void UserInteractionLogger::on_transport_state_change () {
    emit ("transport", _session.transport_rolling () ? "play" : "stop");
}
void UserInteractionLogger::on_located () {
    emit ("transport", "locate");
}
void UserInteractionLogger::on_region_added (std::weak_ptr<Region>) {
    emit ("edit", "region_added");
}
void UserInteractionLogger::on_route_added (RouteList) {
    emit ("mix", "route_added");
}
void UserInteractionLogger::on_record_state_changed () {
    emit ("transport", "record_state_change");
}
```

### 6.3 Connecting the Logger to Existing Signals

To activate the logger, instantiate it in `ARDOUR_UI` after a session is
opened (in `ardour_ui_session.cc`):

```cpp
// In ARDOUR_UI::session_loaded() or similar:
_interaction_logger = std::make_unique<ARDOUR::UserInteractionLogger> (*_session);
_interaction_logger->set_callback ([](const ARDOUR::InteractionEvent& e) {
    // Example: write JSON lines to a file
    // { "t": 12345, "pos": 44100, "cat": "transport", "action": "play" }
    std::ofstream log_file ("/tmp/ardour_interactions.jsonl",
                            std::ios::app);
    log_file << "{\"t\":" << e.wall_clock_ms
             << ",\"pos\":" << e.session_position
             << ",\"cat\":\"" << e.category << "\""
             << ",\"action\":\"" << e.action << "\""
             << ",\"detail\":" << (e.detail.empty() ? "null" : "\"" + e.detail + "\"")
             << "}\n";
});
```

This produces a structured **newline-delimited JSON (NDJSON)** log that any
Python/Node.js AI data pipeline can consume.

For a richer integration (e.g. sending events to a local REST API or gRPC
stream), replace the `std::ofstream` block with an HTTP POST or a WebSocket
send.

---

## 7. Session Management

The `Session` class (`libs/ardour/ardour/session.h`) is the central god-object.
Its implementation is split across ~12 source files:

| File | Content |
|---|---|
| `session.cc` | Constructor, process callback, graph execution |
| `session_state.cc` | XML save / load, snapshot management |
| `session_events.cc` | Event queue processing (`process_event`) |
| `session_rtevents.cc` | Real-time-safe event dispatching |
| `session_transport.cc` | Transport state machine (play / stop / record / loop) |
| `session_midi.cc` | MIDI clock / MTC generation and reception |
| `session_playlists.cc` | Playlist management operations |
| `session_process.cc` | Per-cycle process graph traversal |
| `session_butler.cc` | Butler (background I/O) thread management |
| `session_click.cc` | Click (metronome) track generation |
| `session_ltc.cc` | Linear Timecode encode / decode |
| `session_vst.cc` | VST-specific session hooks |

**Session file format:**

Ardour sessions are saved as `.ardour` XML files.  Every session object
(`Route`, `Region`, `Playlist`, `Location`, …) serialises itself using
`PBD::XMLNode`.  The schema is versioned; `session_state.cc` contains
migration code for older formats.

---

## 8. Audio Engine & Backends

```
AudioEngine  (libs/ardour/ardour/audioengine.h)
│
├── AudioBackend*   (selected at runtime from libs/backends/)
│   ├── JACKAudioBackend
│   ├── ALSAAudioBackend
│   ├── CoreAudioBackend
│   ├── PortAudioBackend
│   ├── PulseAudioBackend
│   └── DummyAudioBackend
│
├── PortManager     (port create/destroy/connect)
│   ├── AudioPort
│   └── MidiPort
│
├── Process callback path:
│   AudioEngine::process_callback()
│     └── Session::process()
│           └── Session::process_routes()  (graph.hpp topological sort)
│                 └── Route::process_output_buffers()
│                       └── Processor::run()  (per plugin/amp/meter)
│
└── Latency:
    ├── MTDM    — hardware round-trip latency measurement
    └── MIDIDM  — MIDI round-trip latency measurement
```

The backend fires a periodic **process callback** at the configured buffer size.
The `Session` processes the entire routing graph within that callback.  All
operations in this path must be **real-time safe** (no heap allocation, no
system calls, no blocking).

---

## 9. Scripting & Automation API (Lua)

Ardour embeds Lua 5.3 (`libs/lua/`) and exposes the entire session object graph
to scripts.

**Script types** (from `ardour/luascripting.h`):

| Type | When it runs | Typical use |
|---|---|---|
| `Session` | Once when loaded | Configure routes, connections, plugins |
| `DSP` | Every audio cycle | Custom audio effect (like a plugin) |
| `EditorAction` | On menu/key trigger | Batch editing, report generation |
| `EditorHook` | On editor events | React to selection, view changes |
| `Snippet` | On demand | One-off operations |

**Key Lua-accessible objects:**

```lua
Session          -- the main Session object
Session:route_by_name("Master")    -- get a route
Session:transport_play()            -- start playback
Session:get_routes()                -- iterator over all routes

Route:gain_control()                -- fader Controllable
Route:rec_enable_control()
Route:mute_control()

Region:name()
Region:start()
Region:length()

ARDOUR.Editor                       -- editor actions (from EditorAction scripts)
```

Scripts live in `share/scripts/` and can be managed from the Lua Script Manager
dialog (`gtk2_ardour/lua_script_manager.h/cc`).

---

## 10. Control Surfaces (`libs/surfaces/`)

See §2.10 for the full list of supported surfaces.  The architecture is:

```
ControlProtocolManager            (ardour/control_protocol_manager.h)
│  (discovers and loads surface plugins)
│
└── ControlProtocol  ──► BasicUI  ──► Session
     │   (abstract)
     │
     └── Concrete implementations:
         MackieControlProtocol
         OSC
         GenericMidi
         FaderPort
         Push2Protocol
         LaunchpadPro
         …
```

Each surface plugin is a shared library loaded at runtime.  The
`ControlProtocolManager` discovers them by scanning known directories and
reads their metadata.

The **OSC surface** (`libs/surfaces/osc/`) deserves special mention: because
it uses standard UDP/TCP networking it can be driven by any third-party
application — including Python AI agents — without any modification to the
Ardour source code.  The OSC namespace is documented in `share/osc/`.

---

## 11. Build System & Configuration

Ardour uses **WAF** (a Python-based build system):

| File | Role |
|---|---|
| `wscript` | Top-level build script; discovers dependencies, configures features |
| `waf` | The WAF executable |
| `tools/` | WAF plugins for specific checks (LV2, VST3, CoreAudio, etc.) |

Building from source:

```bash
./waf configure --prefix=/usr
./waf
./waf install
```

Key configure flags:

| Flag | Effect |
|---|---|
| `--with-backend=jack,alsa` | Select audio backends |
| `--no-vst` | Disable VST2 support |
| `--no-vst3` | Disable VST3 support |
| `--luashell` | Enable the Lua REPL session shell (`luasession/`) |
| `--address-sanitizer` | Enable ASan for debugging |

---

## 12. Shared Resources (`share/`)

```
share/
├── scripts/         50+ bundled Lua scripts (DSP, session helpers, analyzers)
├── templates/       Session templates (empty, podcast, film-score, …)
├── midi_maps/       XML MIDI controller map definitions
├── patchfiles/      Plugin preset bank collections
├── osc/             OSC surface configuration and namespace reference
├── web_surfaces/    HTML/JS/CSS browser-based control surfaces
│                    (served by the WebSockets surface plugin)
├── rdf/             RDF metadata for plugin indexing
├── plugin_metadata/ Cached plugin database (ratings, tags)
├── export/          Export format presets
├── media/           Stock audio media (click sounds, demo content)
└── us2400/          US-2400 surface-specific resources
```

---

## 13. Quick Reference Table

| Module / Path | Purpose | AI / Event hook? |
|---|---|---|
| `libs/ardour/ardour/session.h` | Central session god-object | ✅ Many PBD signals |
| `libs/ardour/ardour/session_event.h` | RT transport event queue | ✅ Intercept `process_event()` |
| `libs/pbd/pbd/signals.h` | Signal/slot IPC | ✅ Foundation for all hooks |
| `libs/pbd/pbd/event_loop.h` | Thread-safe callback scheduling | ✅ Cross-thread event delivery |
| `libs/pbd/pbd/error.h` | Severity-leveled logging | ✅ Register custom stream |
| `libs/ctrl-interface/…/basic_ui.h` | High-level DAW control API | ✅ Static signals for controller actions |
| `libs/surfaces/osc/` | OSC network control surface | ✅ Ready-to-use for AI agents |
| `libs/surfaces/websockets/` | WebSocket control surface | ✅ Browser / Node.js AI agents |
| `gtk2_ardour/ardour_ui.h` | Main GUI application object | — Main event loop |
| `gtk2_ardour/editor.h` | Timeline editor | — All editing operations |
| `gtk2_ardour/mixer_ui.h` | Mixing console | — Mix operations |
| `gtk2_ardour/midi_tracer.h` | Live MIDI event monitor | ✅ MIDI interaction stream |
| `gtk2_ardour/color_theme_manager.h` | Theme hot-swap | — Theming |
| `gtk2_ardour/ui_config.h` | All visual preferences | — Theming |
| `gtk2_ardour/themes/*.colors` | XML color palettes | — Theming |
| `gtk2_ardour/clearlooks.rc.in` | GTK2 widget geometry | — Theming |
| `share/scripts/` | Lua scripts (50+) | ✅ `EditorHook` type for event intercept |
| `luasession/` | Headless Lua session scripting | ✅ Full session API in Lua |
| `libs/audiographer/` | Export analysis pipeline | ✅ Loudness / spectrum data |
| `libs/backends/` | Audio I/O (JACK, ALSA, …) | — Audio hardware abstraction |
| `libs/surfaces/mackie/` | MCU hardware control | — Hardware control |
| `libs/surfaces/push2/` | Ableton Push 2 | — Hardware control |
| `gtk2_ardour/dsp_stats_ui.h` | Per-plugin CPU meters | ✅ Performance monitoring |
| `gtk2_ardour/pingback.h` | Opt-in version check | — Minimal network use |

---

*Document generated for the Ardour repository at `agmada-asa/ardour`.  
Last updated: 2026-03-26.*
