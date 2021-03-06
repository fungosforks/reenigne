/*
If a file is 160KB, 180KB, 320KB, 360KB, 720KB, 1440KB or 2880KB, just interpret it as raw data. If it's a structured file, make sure it is not one of these sizes by adding an extra padding byte if
necessary.

Otherwise, the structured file format is as follows:

Magic bytes: RDIF
Version word: Only 0 currently defined.
File compression word: 0 for uncompressed data follows, 1 for zlib compressed data follows
Creator string pointer word
Creator string length word
Label string pointer word
Label string length word
Description string pointer word
Description string length word
Medium word: 0 = 8" disk, 1 = 5.25" disk, 2 = 3.5" disk
Tracks per inch word: 48, 96, 100
Write enable word
RPM word
Bit rate word
FM/MFM word
Default number of bytes per sector
Default number of sectors per track

File base: this is offset 0 in the file
Number of entries in Block table word: N
Block table: N entries of:
  Offset word: relative to file base
  Size word: in bytes when uncompressed. Overlapping the index hole causes the next part to go onto the following track.
  Cylinder word: 24.8 signed fixed point, relative to track 0
  Head word: Should be an integer
  Track position word: .32 fixed point in revolutions, relative to index hole
  Data rate word: 24.8 bits per revolution
  Type word:
    0 = just the data as 512 byte sectors (logical, not physical order)
    1 = data including gaps
    2 = FM/MFM flux-reversal data (two bits per one actual data bit)
    3 = raw flux measurement (4 bytes per one actual data bit)
  Block compression word: same meaning as file compression word

Block table should be kept sorted in order of cylinder major, head middle, track position minor

Raw block data chunks follow, should be kept sorted in same order as block table.


Look at http://www.kryoflux.com/
  http://www.softpres.org/_media/files:ipfdoc102a.zip

Laser holes: type 3, -128 (or 0?) represents media missing
*/

class DiskImage
{
public:
    DiskImage() { }
    DiskImage(const File& file) { deserialize(file); }
    void deserialize(const File& file)
    {
    }
    void serialize(const File& file)
    {
    }
private:
    File _file;
};
