EXE		= quickbms
# -m32 because QuickBMS has been tested only on 32bit systems and gives problems using 64bit native code
CFLAGS	+= -m32 -s -O2 -fstack-protector-all
PREFIX	= /usr/local
BINDIR	= $(PREFIX)/bin
SRC		= $(EXE).c

all:
	$(CC) $(CFLAGS) -c compression/blast.c
	$(CC) $(CFLAGS) -c libs/lzma/LzmaDec.c
	$(CC) $(CFLAGS) -c libs/lzma/Lzma2Dec.c
	$(CC) $(CFLAGS) -c libs/lzma/Bra86.c
	$(CC) $(CFLAGS) -c libs/lzma/LzFind.c
	#$(CC) $(CFLAGS) -c libs/lzma/LzFindMt.c
	$(CC) $(CFLAGS) -c libs/lzma/LzmaEnc.c -D_7ZIP_ST
	#$(CC) $(CFLAGS) -c libs/lzma/MtCoder.c
	#$(CC) $(CFLAGS) -c libs/lzma/Threads.c
	$(CC) $(CFLAGS) -c encryption/tea.c
	$(CC) $(CFLAGS) -c encryption/xtea.c
	$(CC) $(CFLAGS) -c encryption/xxtea.c
	$(CC) $(CFLAGS) -c compression/unlzx.c
	$(CC) $(CFLAGS) -c compression/unlzwx.c
	$(CC) $(CFLAGS) -c compression/unmspack.c
	$(CC) $(CFLAGS) -c libs/mspack/lzxd.c
	$(CC) $(CFLAGS) -c libs/mspack/mszipd.c
	$(CC) $(CFLAGS) -c libs/mspack/qtmd.c
	$(CC) $(CFLAGS) -c libs/mspack/lzssd.c
	$(CC) $(CFLAGS) -c libs/mspack/mslzhd.c
	$(CC) $(CFLAGS) -c compression/unlzw.c
	$(CC) $(CFLAGS) -c compression/sflcomp.c
	$(CC) $(CFLAGS) -c compression/uncompress.c
	$(CC) $(CFLAGS) -c compression/undmc.c
	$(CC) $(CFLAGS) -c encryption/twofish.c
	$(CC) $(CFLAGS) -c encryption/seed.c
	$(CC) $(CFLAGS) -c encryption/serpent.c
	$(CC) $(CFLAGS) -c compression/lzhuf.c
	$(CC) $(CFLAGS) -c compression/lzari.c
	$(CC) $(CFLAGS) -c compression/bpd.c
	$(CC) $(CFLAGS) -c compression/quicklz.c
	$(CC) $(CFLAGS) -c encryption/ice.c
	$(CC) $(CFLAGS) -c compression/doomhuff.c
	$(CC) $(CFLAGS) -c compression/lzf_d.c
	$(CC) $(CFLAGS) -c compression/lzf_c.c
	$(CC) $(CFLAGS) -c compression/de_lzah.c
	$(CC) $(CFLAGS) -c compression/de_lzh.c
	$(CC) $(CFLAGS) -c compression/unshrink.c
	$(CC) $(CFLAGS) -fno-exceptions -fno-rtti -fno-threadsafe-statics -c compression/unquad.cpp
	$(CC) $(CFLAGS) -fno-exceptions -fno-rtti -fno-threadsafe-statics -c compression/unbalz.cpp
	$(CC) $(CFLAGS) -fno-exceptions -fno-rtti -fno-threadsafe-statics -c libs/ppmd/unppmdi.cpp
	$(CC) $(CFLAGS) -fno-exceptions -fno-rtti -fno-threadsafe-statics -c libs/ppmd/unppmdg.cpp
	$(CC) $(CFLAGS) -fno-exceptions -fno-rtti -fno-threadsafe-statics -c libs/ppmd/unppmdh.cpp
	$(CC) $(CFLAGS) -fno-exceptions -fno-rtti -fno-threadsafe-statics -c libs/ppmd/unppmdj.cpp
	$(CC) $(CFLAGS) -c compression/aplib_depacks.c
	$(CC) $(CFLAGS) -c libs/brieflz/depacks.c
	$(CC) $(CFLAGS) -c libs/brieflz/brieflz.c
	#$(CC) $(CFLAGS) -fno-exceptions -fno-rtti -fno-threadsafe-statics -c compression/unpaq6v2.cpp
	$(CC) $(CFLAGS) -c compression/shclib.c
	$(CC) $(CFLAGS) -c compression/hstest.c
	$(CC) $(CFLAGS) -c compression/sixpack.c
	$(CC) $(CFLAGS) -c compression/ashford.c
	$(CC) $(CFLAGS) -c compression/shc.c
	$(CC) $(CFLAGS) -c compression/unjam.c
	$(CC) $(CFLAGS) -c compression/lzhxlib.c
	$(CC) $(CFLAGS) -c compression/srank.c
	$(CC) $(CFLAGS) -c libs/zziplib/block.c
	$(CC) $(CFLAGS) -c libs/zziplib/bwt.c
	$(CC) $(CFLAGS) -c libs/zziplib/coding.c
	$(CC) $(CFLAGS) -c libs/zziplib/struct_model0.c
	$(CC) $(CFLAGS) -c libs/zziplib/struct_model1.c
	$(CC) $(CFLAGS) -c compression/scexpand.c
	$(CC) $(CFLAGS) -c compression/bpe2.c
	$(CC) $(CFLAGS) -c libs/bcl/huffman.c
	$(CC) $(CFLAGS) -c libs/bcl/lz.c
	$(CC) $(CFLAGS) -c libs/bcl/rice.c
	$(CC) $(CFLAGS) -c libs/bcl/rle.c
	$(CC) $(CFLAGS) -c libs/bcl/shannonfano.c
	$(CC) $(CFLAGS) -c compression/scz_decompress_lib.c
	$(CC) $(CFLAGS) -c libs/szip/sz_api.c
	$(CC) $(CFLAGS) -c libs/szip/rice.c -o rice2.o
	$(CC) $(CFLAGS) -c compression/sr3c.c
	$(CC) $(CFLAGS) -c compression/huffmanlib.c
	$(CC) $(CFLAGS) -c compression/yuke_bpe.c
	$(CC) $(CFLAGS) -c libs/lzhl/Huff.cpp
	$(CC) $(CFLAGS) -c libs/lzhl/Lz.cpp
	$(CC) $(CFLAGS) -c libs/lzhl/Lzhl.cpp
	$(CC) $(CFLAGS) -c compression/d3101.c
	$(CC) $(CFLAGS) -c compression/usq.c
	$(CC) $(CFLAGS) -c compression/lzrw3-a.c
	$(CC) $(CFLAGS) -c libs/tdcb/ahuff.c
	$(CC) $(CFLAGS) -c libs/tdcb/arith.c
	$(CC) $(CFLAGS) -c libs/tdcb/arith1.c
	$(CC) $(CFLAGS) -c libs/tdcb/arith1e.c
	$(CC) $(CFLAGS) -c libs/tdcb/arith-n.c
	$(CC) $(CFLAGS) -c libs/tdcb/compand.c
	$(CC) $(CFLAGS) -c libs/tdcb/huff.c
	$(CC) $(CFLAGS) -c libs/tdcb/lzss.c
	$(CC) $(CFLAGS) -c libs/tdcb/lzw12.c
	$(CC) $(CFLAGS) -c libs/tdcb/lzw15v.c
	$(CC) $(CFLAGS) -c libs/tdcb/silence.c
	$(CC) $(CFLAGS) -c libs/tdcb/mn_incs.c
	$(CC) $(CFLAGS) -c compression/dmc2.c
	$(CC) $(CFLAGS) -c compression/libLZR.c
	$(CC) $(CFLAGS) -c compression/gz_unpack.c
	$(CC) $(CFLAGS) -c encryption/3way.c
	$(CC) $(CFLAGS) -c encryption/skipjack.c
	$(CC) $(CFLAGS) -c encryption/anubis.c
	$(CC) $(CFLAGS) -c encryption/aria.c
	$(CC) $(CFLAGS) -c encryption/crypton.c
	$(CC) $(CFLAGS) -c encryption/frog.c
	$(CC) $(CFLAGS) -c encryption/gost.c
	$(CC) $(CFLAGS) -c encryption/lucifer.c
	$(CC) $(CFLAGS) -c encryption/mars.c
	$(CC) $(CFLAGS) -c encryption/misty1.c
	$(CC) $(CFLAGS) -c encryption/NoekeonDirectRef.c
	$(CC) $(CFLAGS) -c encryption/seal.c
	$(CC) $(CFLAGS) -c encryption/safer.c
	$(CC) $(CFLAGS) -c compression/alone_unpack.c
	$(CC) $(CFLAGS) -c compression/S-Decompressor.c
	$(CC) $(CFLAGS) -c compression/E-Decompressor.c
	$(CC) $(CFLAGS) -c compression/K-Decompressor.c
	$(CC) $(CFLAGS) -c compression/N-Decompressor.c
	$(CC) $(CFLAGS) -c encryption/libkirk/crypto.c
	$(CC) $(CFLAGS) -c encryption/libkirk/kirk_engine.c
	$(CC) $(CFLAGS) -c compression/nitroCompLib.c
	$(CC) $(CFLAGS) -c disasm/disasm.c
	$(CC) $(CFLAGS) -c disasm/cmdlist.c
	$(CC) $(CFLAGS) -c disasm/assembl/assembl.c
	$(CC) $(CFLAGS) -c compression/lzpxj.cpp
	$(CC) $(CFLAGS) -c compression/lzmat_dec.c
	$(CC) $(CFLAGS) -c compression/xu4_lzw.c
	$(CC) $(CFLAGS) -c compression/uo_iris.cpp
	$(CC) $(CFLAGS) -c libs/iris/iris_decompress.cpp
	$(CC) $(CFLAGS) -c libs/iris/iris_huffman.cpp
	$(CC) $(CFLAGS) -c libs/iris/iris_uo_huffman.cpp 
	$(CC) $(CFLAGS) -c libs/iris/iris_btree.cpp
	$(CC) $(CFLAGS) -c libs/old_cabextract/lzx.c
	$(CC) $(CFLAGS) -c libs/mrci/MRCI.cpp
	$(CC) $(CFLAGS) -c libs/mrci/GrowBuf.cpp
	$(CC) $(CFLAGS) -c libs/lz4/lz4.c
	$(CC) $(CFLAGS) -c libs/lz4/lz4hc.c
	$(CC) $(CFLAGS) -c compression/rnc.c
	$(CC) $(CFLAGS) -c compression/dernc.c
	$(CC) $(CFLAGS) -c compression/_rnc.c
	$(CC) $(CFLAGS) -c compression/lzv1.c
	$(CC) $(CFLAGS) -c compression/fastlz.c
	$(CC) $(CFLAGS) -c compression/shrinker.c
	$(CC) $(CFLAGS) -c compression/scummvm.c
	$(CC) $(CFLAGS) -c libs/mmini/mmini_huffman.c
	$(CC) $(CFLAGS) -c libs/mmini/mmini_lzl.c
	$(CC) $(CFLAGS) -c libs/clzw/lzw-dec.c
	#$(CC) $(CFLAGS) -c compression/unlpaq8.cpp
	$(CC) $(CFLAGS) -c libs/lzlib/lzlib.c
	$(CC) $(CFLAGS) -c libs/mydownlib/mydownlib.c
	$(CC) $(CFLAGS) -c libs/snappy/snappy.cc
	$(CC) $(CFLAGS) -c libs/snappy/snappy-c.cc
	$(CC) $(CFLAGS) -c libs/snappy/snappy-stubs-internal.cc
	$(CC) $(CFLAGS) -c libs/snappy/snappy-sinksource.cc
	$(CC) $(CFLAGS) -c compression/pucrunch.c
	$(CC) $(CFLAGS) -c libs/libzpaq/unzpaq.cpp
	$(CC) $(SRC) $(CFLAGS) -o $(EXE) *.o -ldl -lcrypto -lssl -lz -lbz2 -lstdc++ -lm -lpthread -llzo2 -DDISABLE_UCL -DDISABLE_MCRYPT -DDISABLE_TOMCRYPT
	# $(CC) $(SRC) $(CFLAGS) -o $(EXE) *.o -ldl -lcrypto -lssl -lz -lbz2 -lstdc++ -lm -lpthread -llzo2 -lucl -lmcrypt -ltomcrypt # you need these 4 libraries

install:
	install -m 755 -d $(BINDIR)
	install -m 755 $(EXE) $(BINDIR)/$(EXE)

.PHONY:
	install
