# vim:ai:et:ff=unix:fileencoding=utf-8:sw=4:ts=4:
# conveyor/src/main/python/conveyor/listener.py
#
# conveyor - Printing dispatch engine for 3D objects and their friends.
# Copyright © 2012 Matthew W. Samsonoff <matthew.samsonoff@makerbot.com>
#
# This program is free software: you can redistribute it and/or modify it under
# the terms of the GNU Affero General Public License as published by the Free
# Software Foundation, either version 3 of the License, or (at your option) any
# later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
# details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

from __future__ import (absolute_import, print_function, unicode_literals)

import errno
import logging
import os
import os.path
import socket

import conveyor.connection
import conveyor.stoppable

class Listener(conveyor.stoppable.Stoppable):
    def __init__(self):
        conveyor.stoppable.Stoppable.__init__(self)
        self._log = logging.getLogger(self.__class__.__name__)

    def accept(self):
        raise NotImplementedError

    def cleanup(self):
        raise NotImplementedError

    def __enter__(self):
        pass

    def __exit__(self, exc_type, exc_value, traceback):
        self.cleanup()
        return False

class _AbstractSocketListener(Listener):
    def __init__(self, socket):
        Listener.__init__(self)
        self._stopped = False
        self._socket = socket

    def stop(self):
        self._stopped = True

    def accept(self):
        self._socket.settimeout(1.0)
        while True:
            if self._stopped:
                return None
            else:
                try:
                    sock, addr = self._socket.accept()
                except socket.timeout:
                    # NOTE: too spammy
                    # self._log.debug('handled exception', exc_info=True)
                    continue
                except IOError as e:
                    if errno.EINTR == e.args[0]:
                        # NOTE: too spammy
                        # self._log.debug('handled exception', exc_info=True)
                        continue
                    else:
                        raise
                else:
                    connection = conveyor.connection.SocketConnection(sock, addr)
                    return connection

class TcpListener(_AbstractSocketListener):
    def cleanup(self):
        pass

if 'nt' != os.name:
    class _PosixPipeListener(_AbstractSocketListener):
        def __init__(self, path, socket):
            _AbstractSocketListener.__init__(self, socket)
            self._path = path

        def cleanup(self):
            if os.path.exists(self._path):
                os.unlink(self._path)

    PipeListener = _PosixPipeListener

else:
    import ctypes
    import ctypes.wintypes
    import conveyor.platform.win32 as win32

    # The size of the pipe read and write buffers.
    _SIZE = 4096

    # The accept polling timeout.
    _TIMEOUT = 1000

    class _Win32PipeListener(Listener):
        def __init__(self, path):
            Listener.__init__(self)
            self._stopped = False
            self._path = path

        def stop(self):
            self._stopped = True

        def accept(self):
            handle = win32.CreateNamedPipe(
                self._path,
                win32.PIPE_ACCESS_DUPLEX | win32.FILE_FLAG_OVERLAPPED,
                win32.PIPE_TYPE_MESSAGE | win32.PIPE_READMODE_MESSAGE | win32.PIPE_WAIT,
                win32.PIPE_UNLIMITED_INSTANCES,
                _SIZE,
                _SIZE,
                0,
                None)
            if win32.INVALID_HANDLE_VALUE == handle:
                error = win32.GetLastError()
                raise win32.create_WindowsError(error)
            else:
                overlapped = conveyor.connection.PipeConnection.createoverlapped()
                result = win32.ConnectNamedPipe(handle, overlapped)
                if not result:
                    error = win32.GetLastError()
                    if win32.ERROR_IO_PENDING == error:
                        pass
                    elif win32.ERROR_PIPE_CONNECTED == error:
                        connection = conveyor.connection.PipeConnection.create(handle)
                        return connection
                    else:
                        raise win32.create_WindowsError(error)
                connection = self._accept_pending(overlapped)
                return connection

        def _accept_pending(self, overlapped):
            while True:
                if self._stopped:
                    return None
                else:
                    result = win32.WaitForSingleObject(overlapped.hEvent, _TIMEOUT)
                    if win32.WAIT_TIMEOUT == result:
                        connection = conveyor.connection.PipeConnection.create(handle)
                        return connection
                    elif win32.WAIT_OBJECT_0 == result:
                        pass
                    else:
                        raise ValueError(result)

        def cleanup(self):
            pass

    PipeListener = _Win32PipeListener
