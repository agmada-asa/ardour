/*
 * Copyright (C) 2026 Ardour Contributors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#pragma once

/**
 * @file user_interaction_logger.h
 *
 * UserInteractionLogger — a non-intrusive observer that records user
 * interactions with a running Session and delivers structured
 * InteractionEvent objects to a caller-supplied callback.
 *
 * This module is the primary hook point for AI / analytics workflows
 * that need a stream of what the user is doing inside Ardour.
 *
 * DESIGN NOTES
 * ============
 * The logger connects to existing PBD signals emitted by Session (and
 * optionally by individual Routes).  No existing code is modified; the
 * logger is purely additive.
 *
 * Signal connections are stored in a ScopedConnectionList, so all
 * subscriptions are automatically cancelled when the logger is destroyed.
 *
 * Thread safety
 * -------------
 * The callback is invoked from the GUI thread (the same thread that emits
 * the Session signals).  If you need to process events on a different
 * thread you must do your own queuing inside the callback.
 *
 * Usage example
 * -------------
 * @code
 *   // After a session is loaded in ARDOUR_UI:
 *   _logger = std::make_unique<ARDOUR::UserInteractionLogger>(*_session);
 *   _logger->set_callback ([](const ARDOUR::InteractionEvent& e) {
 *       std::cout << e.category << "/" << e.action
 *                 << " @ sample " << e.session_position << "\n";
 *   });
 * @endcode
 *
 * Interaction categories produced by the built-in subscriptions
 * -------------------------------------------------------------
 *   "transport"  — play, stop, locate, record_state_change
 *   "mix"        — route_added
 *   "edit"       — region_added (via playlist signals, if subscribed)
 *
 * Additional categories can be emitted manually by calling emit() if a
 * subclass or helper code has access to the logger instance.
 */

#include <functional>
#include <memory>
#include <string>

#include "pbd/signals.h"

#include "ardour/libardour_visibility.h"
#include "ardour/route.h"
#include "ardour/types.h"

namespace ARDOUR {

class Session;

/**
 * A single timestamped user-interaction event.
 */
struct LIBARDOUR_API InteractionEvent {
	/** Monotonic wall-clock time in milliseconds (via g_get_monotonic_time). */
	int64_t     wall_clock_ms;

	/** Transport (playhead) sample position at the time of the event. */
	samplepos_t session_position;

	/**
	 * High-level category string.
	 *   "transport" — play/stop/locate/record
	 *   "mix"       — route changes
	 *   "edit"      — region / playlist changes
	 *   "midi"      — MIDI-specific events (used by subclasses / external code)
	 *   "ui"        — generic UI interactions
	 */
	std::string category;

	/** Short machine-readable action name within the category. */
	std::string action;

	/**
	 * Optional JSON-encoded detail string carrying extra context.
	 * Empty string if not applicable.
	 * Example: "{\"route\":\"Drums\",\"gain_db\":-6.0}"
	 */
	std::string detail;
};

/** Callback invoked for every captured interaction event. */
using InteractionCallback = std::function<void(const InteractionEvent&)>;

/**
 * UserInteractionLogger
 *
 * Subscribes to Session-level PBD signals and delivers InteractionEvent
 * objects to a registered callback.  Instantiate after a Session is loaded
 * and destroy before the Session is closed.
 */
class LIBARDOUR_API UserInteractionLogger
{
public:
	/**
	 * Construct and connect to @p session's signals.
	 * No events are delivered until set_callback() is called.
	 */
	explicit UserInteractionLogger (Session& session);

	/**
	 * Destroy: all signal connections are automatically released via
	 * ScopedConnectionList destructor.
	 */
	~UserInteractionLogger ();

	/** Register (or replace) the callback that receives interaction events. */
	void set_callback (InteractionCallback cb);

	/** Manually emit an event — useful from external code or subclasses. */
	void emit (const std::string& category,
	           const std::string& action,
	           const std::string& detail = std::string ());

private:
	Session&             _session;
	InteractionCallback  _callback;

	/** Holds all PBD signal connections; cleared on destruction. */
	PBD::ScopedConnectionList _connections;

	/* Signal handlers -------------------------------------------------- */
	void on_transport_state_change ();
	void on_located ();
	void on_route_added (RouteList&);
	void on_record_state_changed ();
};

} // namespace ARDOUR
