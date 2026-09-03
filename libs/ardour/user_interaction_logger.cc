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

#include <glib.h>

#include "pbd/event_loop.h"

#include "ardour/session.h"
#include "ardour/user_interaction_logger.h"

using namespace ARDOUR;

/* ------------------------------------------------------------------ */
/*  Constructor / Destructor                                           */
/* ------------------------------------------------------------------ */

UserInteractionLogger::UserInteractionLogger (Session& session)
	: _session (session)
{
	/* Transport play / stop / rolling state */
	_session.TransportStateChange.connect (
		_connections,
		MISSING_INVALIDATOR,
		boost::bind (&UserInteractionLogger::on_transport_state_change, this),
		PBD::EventLoop::get_event_loop_for_thread ());

	/* Locate (jump to position) */
	_session.Located.connect (
		_connections,
		MISSING_INVALIDATOR,
		boost::bind (&UserInteractionLogger::on_located, this),
		PBD::EventLoop::get_event_loop_for_thread ());

	/* Route (track / bus) added */
	_session.RouteAdded.connect (
		_connections,
		MISSING_INVALIDATOR,
		boost::bind (&UserInteractionLogger::on_route_added, this, _1),
		PBD::EventLoop::get_event_loop_for_thread ());

	/* Record arm state changed */
	_session.RecordStateChanged.connect (
		_connections,
		MISSING_INVALIDATOR,
		boost::bind (&UserInteractionLogger::on_record_state_changed, this),
		PBD::EventLoop::get_event_loop_for_thread ());
}

UserInteractionLogger::~UserInteractionLogger ()
{
	/* _connections ScopedConnectionList cleans up all subscriptions. */
}

/* ------------------------------------------------------------------ */
/*  Public API                                                         */
/* ------------------------------------------------------------------ */

void
UserInteractionLogger::set_callback (InteractionCallback cb)
{
	_callback = cb;
}

void
UserInteractionLogger::emit (const std::string& category,
                              const std::string& action,
                              const std::string& detail)
{
	if (!_callback) {
		return;
	}

	InteractionEvent ev;
	ev.wall_clock_ms    = static_cast<int64_t> (g_get_monotonic_time () / 1000);
	ev.session_position = _session.transport_sample ();
	ev.category         = category;
	ev.action           = action;
	ev.detail           = detail;

	_callback (ev);
}

/* ------------------------------------------------------------------ */
/*  Signal handlers                                                    */
/* ------------------------------------------------------------------ */

void
UserInteractionLogger::on_transport_state_change ()
{
	emit ("transport",
	      _session.transport_rolling () ? "play" : "stop");
}

void
UserInteractionLogger::on_located ()
{
	emit ("transport", "locate");
}

void
UserInteractionLogger::on_route_added (RouteList& /*routes*/)
{
	emit ("mix", "route_added");
}

void
UserInteractionLogger::on_record_state_changed ()
{
	emit ("transport", "record_state_change");
}
