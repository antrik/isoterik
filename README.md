#Introduction

isoterik extracts a "purified" `.iso` (UDF) image of a video DVD, devoid of
region code. (And CSS encryption.)

When this image is burned to a writable medium, the resulting disc can be
played in any DVD player, regardless of region setting -- so you no longer need
an extra DVD player just to play imported discs...

Although not the primary motivation, this can also be used to just easily copy
any video DVD on the command line, even if you don't care about region code
removal.

While other Free Software programs that can copy DVDs (such as k3b or Brasero)
creade a new ISO/UDF filesystem from the decrypted video files (using
genisoimage/mkisofs), isoterik keeps the original filesystem structure,
replacing only the actual purified data. The approach used by the other
programs yields slight differences from the orignal at the filesystem level
(such as different file creation dates) -- unless there are some buggy corner
cases in genisoimage and/or some DVD players, this shouldn't really make any
difference in practice though.

Note: isoterik itself doesn't do any CSS decryption. Rather, it invokes
`dvdbackup` to extract decrypted files; which depends on `libdvdread` to do the
actual work; which in turn needs yet another library to deal with CSS, that I
might not even be allowed to tell you about without risking legal
repercussions... Yay for copyright legislation.

#Installation

isoterik consists of two components: the main `isoterik` program, which is a
shell script; and the `udffindfile` helper binary, which wraps a particular
functionality of `libdvdread` that is not otherwise available in any way that
could be used from a shell script.

To get isoterik working, first you need to build the binary. In addition to the
basic build tools (on Debian and derivatives, install the `build-essential`
package), you also need to install the libdvdread development package.
(`libdvdread-dev` on Debian.)

Once the dependencies are in place, on typical GNU/Linux systems just running
`make` in the source directory should be sufficient on yield the `udffindfile`
binary. (On other systems, possibly some adjustments might be necessary to
`Makefile`.)

For isoterik to be able to use the binary, it needs to be made available in the
executable search path -- typically by copying it to `/usr/local/bin` or
`~/bin`. While the actual `isoterik` program can be invoked from the source
directory, I suggest installing it to one of the `bin` directories as well, for
consistency and convenience.

At runtime, isoterik also relies on the `dvdbackup` program, which needs to be
installed separately. (It's available in the `dvdbackup` package in Debian.)
Furthermore -- as mentioned in the introduction -- to actually obtain "clean"
copies of CSS-encrypted video files, the `libdvdread` library underlying
`dvdbackup` needs to have access to a particular extra component...

If your system can already play the DVD you want to purify (using Free Software
video players such as VLC, Totem etc.), then the extra library must already be
present, and isoterik should work just fine. If you can't play the DVD, then
isoterik won't work either, and you need to install the extra library first.

(Or maybe there is a problem with your DVD drive -- in which case there is
nothing you can do, except using a different machine... My notebook for example
fails with at least one DVD having a "foreign" region code, although it plays
other DVDs just fine; while my two desktop drives work with any DVD. I have no
idea how common this is.)

Another runtime dependency is `/sbin/blkid`, which should always be present on
GNU/Linux and similar systems, but might be missing elsewhere.

#Usage

The main program currently doesn't accept any command line arguments -- so all
you can do is simply invoking `isoterik`. This will attempt to access a video
DVD through `/dev/dvd` -- on most systems, this is a link pointing to the
primary DVD drive device. If you need to use a different device, you have to
edit the actual `isoterik` script, changing the value of the `DEV` variable
defined near the top of the script.

(Some other setting variables are also available -- most notably to change the
verbosity of status output.)

If a video DVD is present, isoterik first reads its "volume label", which is a
short string uniquely identifying the DVD -- usually a shortened but
recognisable variant of the official title. It then creates a "purified" image
in the current directory, named after the volume label with `.iso` appended to
it.

During operation, isoterik first creates a temporary directory (also named
after the volume label, but without `.iso` appended) within the current
directory, where it stores decrypted copies of all video files from the DVD.
When assembling the final image file, these temporary files are successively
removed. Still, the partition where the final image and the temporary directory
are stored does need some temporary extra space during processing, on top of
the size of the final image -- in the worst case, up to the maximal size of one
`VOB` file, which is `1 GiB`.

#ToDo

isoterik is still pretty rough around the edges. Possible future improvements:

  * A "proper" build system for the binary, using Autoconf.

  * Command line options -- most notably for selecting the source device; and
    for changing verbosity of status messages.

  * Operation could be optimised by storing the decrypted video contents
    directly to the final image, instead of extracting them to temporary files
    first. This would avoid the need for the temporary directory (and the
    temporary extra space usage); and avoiding the extra copy would also speed
    up the operation considerably.

    This different mode of operation shouldn't be too hard to implement in the
    isoterik script itself -- however, it would require extra functionality
    from the underlying programs: specifically, the ability to extract
    individual video files.

    (And probably also a more explicit way to get a listing of all video files
    on the DVD. While `dvdbackup` can print this information, the output
    doesn't appear suitable for reliable machine parsing...)

    There are several possible approaches for tackling this problem:

      * Use `vobcopy` instead of `dvdbackup`. This is another program doing
        mostly the same, but with some different functions -- including
        extraction of individual files. However, I'm reluctant to introduce a
        dependency on vobcopy, as it seems rather unmaintained, and the code
        quality is horrible...

      * Implement additional helper binaries. However, unlike the `udffindfile`
        functionality, extracting files is a rather complex process, which
        would require duplicating quite a lot of functionality from dvdbackup.

      * Implement the whole thing in C. If we have to implement large parts of
        the core functionality anyways (rather than relying on existing
        programs), there is really not much point in using a wrapper script at
        all...

      * Add the required extra functionality to dvdbackup, and use that in our
        script. This shouldn't be too hard to do -- but it would complicate
        deployment of isoterik, as it would now rely on a new version of
        dvdbackup to work. (Unless we keep the old mode of operation in place
        as a fallback, complicating the script...)

      * Implement the whole thing within dvdbackup: introducing an optional new
        mode of operation doing the same as isoterik...
