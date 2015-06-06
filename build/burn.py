import os
from sys import argv

DISK_FILE = 'disk.img'
FILE_RECORD_LEN = 64
DISK_SIZE = 1440*1024
DISK_ALIGN = 512 

RECORD_FILE = 'filelist.dat'
RECORD_ENABLE = True


def writeat(f, data, offset):
    f.seek(offset)
    f.write(data)

if __name__ == "__main__":
#   initialize the disk and record 
    rec = None 
    current_dir = './'
    disk = open(DISK_FILE, 'wb')
    writeat(disk, '\0'*DISK_SIZE, 0)

    argv.pop(0)
    pos = 0
    for arg in argv:
        if arg.startswith('-'):
            if arg == '-rec=off':  # shutdown the record
                RECORD_ENABLE = False
                if rec: rec.close()

            elif arg == '-rec=on':   # turn on the record
                RECORD_ENABLE = True
                rec = open(RECORD_FILE, 'w')

            elif arg.startswith('-seek='):
                pos = int(arg[6:])

            elif arg.startswith('-cd='):
                if not arg.endswith('/'): arg+='/'
                current_dir += arg[4:]

        else:
            filename = current_dir+arg
            inputf = open(filename, 'rb')

            # burn disk
            size = os.stat(filename).st_size
            writeat(disk, inputf.read(), pos)
            block_count = ((size-1) / DISK_ALIGN + 1)

            print '%s:: start: %d len: %d' % (filename, pos/512, block_count)
            if RECORD_ENABLE:
#                rec.write('"%s\\0%s\\x%s\\x%s"\r\n' % (arg, '.'*(14-len(arg)-1), str(hex(pos/512))[2:], str(hex(block_count))[2:] ) )
                rec.write('%s\0%s%c%c' % (arg, '.'*(FILE_RECORD_LEN-2-len(arg)-1), pos/512, block_count) )
            pos += block_count * DISK_ALIGN
    disk.close();
    rec.close();
    print 'done.'

'''
	python burn.py \
		-rec=off \
		-seek=0 bootstrap.bin \
		-seek=510 55aa.dat \
		-seek=1024 os.bin \
			-rec=on -cd=programs\
				ls.com help.com time.com date.com asc.com reboot.com cls.com \
				testint.com testclib.com\
				shell.com top.com\
				spin1.com spin2.com spin3.com spin4.com\
			-rec=off -cd=..\
		-seek=512 filelist.dat
'''
