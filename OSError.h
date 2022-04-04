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
