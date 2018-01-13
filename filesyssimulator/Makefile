all: mkfs ls cat tee cp dump mkdir ln rm fsck defrag

cp: cp.cc Kernel.o IndexNode.o FileDescriptor.o DirectoryEntry.o ProcessContext.o FileSystem.o Stat.o BitBlock.o Block.o SuperBlock.o
	g++ -g cp.cc Kernel.o IndexNode.o FileDescriptor.o DirectoryEntry.o ProcessContext.o FileSystem.o Stat.o BitBlock.o Block.o SuperBlock.o -o cp

tee: tee.cc Kernel.o IndexNode.o FileDescriptor.o DirectoryEntry.o ProcessContext.o FileSystem.o Stat.o BitBlock.o Block.o SuperBlock.o
	g++ -g tee.cc Kernel.o IndexNode.o FileDescriptor.o DirectoryEntry.o ProcessContext.o FileSystem.o Stat.o BitBlock.o Block.o SuperBlock.o -o tee

ls: ls.cc Stat.o DirectoryEntry.o Kernel.o FileSystem.o FileDescriptor.o ProcessContext.o
	g++ -g ls.cc Stat.o FileDescriptor.o FileSystem.o BitBlock.o IndexNode.o DirectoryEntry.o Block.o SuperBlock.o ProcessContext.o Kernel.o -o ls

mkdir: mkdir.cc Stat.o FileDescriptor.o FileSystem.o BitBlock.o IndexNode.o DirectoryEntry.o Block.o SuperBlock.o ProcessContext.o Kernel.o
	g++ -g mkdir.cc Stat.o FileDescriptor.o FileSystem.o BitBlock.o IndexNode.o DirectoryEntry.o Block.o SuperBlock.o ProcessContext.o Kernel.o -o mkdir

cat: cat.cc Stat.o FileDescriptor.o FileSystem.o BitBlock.o IndexNode.o DirectoryEntry.o Block.o SuperBlock.o ProcessContext.o Kernel.o
	g++ -g cat.cc Stat.o FileDescriptor.o FileSystem.o BitBlock.o IndexNode.o DirectoryEntry.o Block.o SuperBlock.o ProcessContext.o Kernel.o -o cat

ln: ln.cc Stat.o FileDescriptor.o FileSystem.o BitBlock.o IndexNode.o DirectoryEntry.o Block.o SuperBlock.o ProcessContext.o Kernel.o
	g++ -g ln.cc Stat.o FileDescriptor.o FileSystem.o BitBlock.o IndexNode.o DirectoryEntry.o Block.o SuperBlock.o ProcessContext.o Kernel.o -o ln

rm: rm.cc Stat.o FileDescriptor.o FileSystem.o BitBlock.o IndexNode.o DirectoryEntry.o Block.o SuperBlock.o ProcessContext.o Kernel.o
	g++ -g rm.cc Stat.o FileDescriptor.o FileSystem.o BitBlock.o IndexNode.o DirectoryEntry.o Block.o SuperBlock.o ProcessContext.o Kernel.o -o rm

fsck: fsck.cc Stat.o FileDescriptor.o FileSystem.o BitBlock.o IndexNode.o DirectoryEntry.o Block.o SuperBlock.o ProcessContext.o Kernel.o
	g++ -g fsck.cc Stat.o FileDescriptor.o FileSystem.o BitBlock.o IndexNode.o DirectoryEntry.o Block.o SuperBlock.o ProcessContext.o Kernel.o -o fsck

defrag: defrag.cc Stat.o FileDescriptor.o FileSystem.o BitBlock.o IndexNode.o DirectoryEntry.o Block.o SuperBlock.o ProcessContext.o Kernel.o
	g++ -g defrag.cc Stat.o FileDescriptor.o FileSystem.o BitBlock.o IndexNode.o DirectoryEntry.o Block.o SuperBlock.o ProcessContext.o Kernel.o -o defrag

mkfs: mkfs.cc BitBlock.o IndexNode.o DirectoryEntry.o Block.o SuperBlock.o
mkfs: mkfs.cc BitBlock.o IndexNode.o DirectoryEntry.o Block.o SuperBlock.o
	g++ -g mkfs.cc BitBlock.o IndexNode.o DirectoryEntry.o Block.o SuperBlock.o -o mkfs
mkfs: mkfs.cc BitBlock.o IndexNode.o DirectoryEntry.o Block.o SuperBlock.o
	g++ -g mkfs.cc BitBlock.o IndexNode.o DirectoryEntry.o Block.o SuperBlock.o -o mkfs

dump: dump.cc
	g++ -g dump.cc -o dump

Kernel.o: Kernel.cc
	g++ -g -c Kernel.cc

ProcessContext.o: ProcessContext.cc
	g++ -g -c ProcessContext.cc 

FileDescriptor.o: FileDescriptor.cc
	g++ -g -c FileDescriptor.cc
	
FileSystem.o: FileSystem.cc 
	g++ -g -c FileSystem.cc 

Stat.o: Stat.cc
	g++ -g -c Stat.cc

DirectoryEntry.o: DirectoryEntry.cc
	g++ -g -c DirectoryEntry.cc

BitBlock.o: BitBlock.cc
	g++ -g -c BitBlock.cc

IndexNode.o: IndexNode.cc
	g++ -g -c IndexNode.cc 

Block.o: Block.cc
	g++ -g -c Block.cc

SuperBlock.o: SuperBlock.cc	
	g++ -g -c SuperBlock.cc

clean:
	rm *.o mkfs ls cat tee cp dump mkdir ln rm fsck defrag

