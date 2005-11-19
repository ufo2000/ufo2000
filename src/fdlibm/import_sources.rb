# Ruby script used to import fdlibm sources from gcc 3.3.6 distribution
# which automatically adds 'fdlibm' namespace to all the needed sources
# and renames them to *.cpp extension

filelist = %w{
    e_acos.c e_asin.c e_atan2.c e_exp.c e_fmod.c e_log.c e_pow.c e_rem_pio2.c e_remainder.c
    e_scalb.c e_sqrt.c k_cos.c k_rem_pio2.c k_sin.c k_tan.c s_atan.c s_ceil.c s_copysign.c
    s_cos.c s_fabs.c s_floor.c s_rint.c s_scalbn.c s_sin.c s_tan.c w_acos.c w_asin.c
    w_atan2.c w_exp.c w_fmod.c w_log.c w_pow.c w_remainder.c w_sqrt.c    
}

filelist.each {|filename|
    puts filename
    buf = File.open(filename, "r").read
    buf.strip!
    buf.sub!(/\#include \"fdlibm\.h\"/, 
        "\#include \"fdlibm\.h\"\nusing namespace fdlibm;\nnamespace fdlibm {")
    fh = File.open(filename + "pp", "w")
    fh.write(buf)
    fh.write("\n}\n")
    fh.close()
}
