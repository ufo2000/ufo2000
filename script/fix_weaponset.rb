#!/usr/bin/env ruby

##############################################################################
# Script for fixing inconsistent names for all referenced .png files
##############################################################################

if not ARGV[0] then
    printf("Usage: fix_reaponset.rb [weaponset_directory]\n")
    exit(0)
end

require 'find'

filenames_table = {}

# recursively walk weaponset directory, get list of png files, also 
# rename png files to lowercase
Find.find(ARGV[0]) { |f| 
    # ignore .svn subdirectories
    next if f =~ /((^)|([\/]))\.svn(([\/])|($))/
    # for *.png ensure that their extension is in lowercase, also try to run
    # optipng for each png file
    if f =~ /\.(png)$/i then
        fixed_name = f.sub(/\.png$/i, ".png")
        if f != fixed_name then 
            printf("renaming %s to %s\n", f, fixed_name)
            File.rename(f, fixed_name) 
        end
        prefixed_name = fixed_name[ARGV[0].length .. -1]
        if prefixed_name[0 .. 0] != '/' then prefixed_name = "/" + prefixed_name end
        prefixed_name = "$(extension)" + prefixed_name
        filenames_table[prefixed_name.downcase] = prefixed_name
    end
}

# walk top level directory and search for lua files, rename lua files to 
# lowercase, in each lua file find references to files and fix filename case
# if required
Find.find(ARGV[0]) { |f|
    next if not f =~ /\.(lua)$/i
    fixed_name = f.sub(/\.lua$/i, ".lua")
    if f != fixed_name then 
        printf("renaming %s to %s\n", f, fixed_name)
        File.rename(f, fixed_name) 
    end
    f = File.open(fixed_name, "rb")
    text = f.read
    f.close
    text.gsub!(/(png_image\(\s*\")([^\"]*)(\"\s*\))/) {|x|
        prefix = $1
        filename = $2
        suffix = $3

        if filename =~ /(\/\d+x\d+\-\d+\.png)$/i then
            extpart = $1
            filename = filename[0 .. -extpart.length - 1] + ".png"
        else
            extpart = ".png"
        end

        if filenames_table[filename.downcase] and filenames_table[filename.downcase] != filename then
            printf("%s -> %s\n", filename, filenames_table[filename.downcase].sub(/\.png$/i, extpart))
            prefix + filenames_table[filename.downcase].sub(/\.png$/i, extpart) + suffix
        else
            x
        end
    }
    f = File.open(fixed_name, "wb")
    f.write(text)
    f.close
}
