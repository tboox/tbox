/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
/* this header is used to define signal constants and names; it might be included several times */
#ifndef __BIONIC_SIGDEF
#error __BIONIC_SIGDEF not defined
#endif

__BIONIC_SIGDEF(SIGHUP,1,"Hangup")
__BIONIC_SIGDEF(SIGINT,2,"Interrupt")
__BIONIC_SIGDEF(SIGQUIT,3,"Quit")
__BIONIC_SIGDEF(SIGILL,4,"Illegal instruction")
__BIONIC_SIGDEF(SIGTRAP,5,"Trap")
__BIONIC_SIGDEF(SIGABRT,6,"Aborted")
__BIONIC_SIGDEF(SIGBUS,7,"Bus error")
__BIONIC_SIGDEF(SIGFPE,8,"Floating point exception")
__BIONIC_SIGDEF(SIGKILL,9,"Killed")
__BIONIC_SIGDEF(SIGUSR1,10,"User signal 1")
__BIONIC_SIGDEF(SIGSEGV,11,"Segmentation fault")
__BIONIC_SIGDEF(SIGUSR2,12,"User signal 2")
__BIONIC_SIGDEF(SIGPIPE,13,"Broken pipe")
__BIONIC_SIGDEF(SIGALRM,14,"Alarm clock")
__BIONIC_SIGDEF(SIGTERM,15,"Terminated")
__BIONIC_SIGDEF(SIGSTKFLT,16,"Stack fault")
__BIONIC_SIGDEF(SIGCHLD,17,"Child exited")
__BIONIC_SIGDEF(SIGCONT,18,"Continue")
__BIONIC_SIGDEF(SIGSTOP,19,"Stopped (signal)")
__BIONIC_SIGDEF(SIGTSTP,20,"Stopped")
__BIONIC_SIGDEF(SIGTTIN,21,"Stopped (tty input)")
__BIONIC_SIGDEF(SIGTTOU,22,"Stopper (tty output)")
__BIONIC_SIGDEF(SIGURG,23,"Urgent I/O condition")
__BIONIC_SIGDEF(SIGXCPU,24,"CPU time limit exceeded")
__BIONIC_SIGDEF(SIGXFSZ,25,"File size limit exceeded")
__BIONIC_SIGDEF(SIGVTALRM,26,"Virtual timer expired")
__BIONIC_SIGDEF(SIGPROF,27,"Profiling timer expired")
__BIONIC_SIGDEF(SIGWINCH,28,"Window size changed")
__BIONIC_SIGDEF(SIGIO,29,"I/O possible")
__BIONIC_SIGDEF(SIGPWR,30,"Power failure")
__BIONIC_SIGDEF(SIGSYS,31,"Bad system call")

#undef __BIONIC_SIGDEF
