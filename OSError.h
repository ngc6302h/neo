/*
    Copyright (C) 2022  Iori Torres (shortanemoia@protonmail.com)
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include <errno.h>

namespace neo
{
    enum class OSError
    {
        Success = 0,
        OperationNotPermitted = EPERM,
        NoSuchEntity = ENOENT,
        NoSuchProcess = ESRCH,
        IOError = EIO,
        NoSuchFileOrAddress = ENXIO,
        ArgumentListTooLong = E2BIG,
        ExecFormatError = ENOEXEC,
        BadFileNumber = EBADF,
        NoChildProcesses = ECHILD,
        TryAgain = EAGAIN,
        OutOfMemory = ENOMEM,
        PermissionDenied = EACCES,
        BadAddress = EFAULT,
        BlockDeviceRequired = ENOTBLK,
        DeviceOrResourceBusy = EBUSY,
        FileExists = EEXIST,
        CrossDeviceLink = EXDEV,
        NoSuchDevice = ENODEV,
        NotADirectory = ENOTDIR,
        IsADirectory = EISDIR,
        InvalidArgument = EINVAL,
        FileTableOverflow = ENFILE,
        TooManyOpenFiles = EMFILE,
        NotATypewriter = ENOTTY,
        TextFileBusy = ETXTBSY,
        FileTooLarge = EFBIG,
        NoSpaceLeftOnDevice = ENOSPC,
        IllegalSeek = ESPIPE,
        ReadOnlyFilesystem = EROFS,
        TooManyLinks = EMLINK,
        BrokenPipe = EPIPE,
        ArgumentOutOfDomain = EDOM,
        MathResultNotRepresentable = ERANGE,
        _CustomErrorStart = 1024,
        EndOfFile = 1025
    };
}
using neo::OSError;
