-- !!! Modified version of lumikki (added support for lua sources 
--     syntax highlighting)
--
-- FILTERS.LUA      Lumikki built-in filter functions
--
-- Note: This file should be read in by '-l filters.lua' as part of the Lua
--       command line to launch Lumikki. This in order to allow easy modifications
--       of the file, and/or extend its features in another .lua file (you can
--       read in as many libraries as you want).
-- 
-- To-do:
--      ImageMagick version detection (require 6.x)
--
-- AK(20-Oct-04): Started making 'covert' for <@GRA..>
--

function LUASOURCE(tbl, text)
    local rc, sout, serr = os.execute("source-highlight -f html -s lua -c x --no-doc", 3, text)
    if rc ~= 0 then sout = "<pre>" .. text .. "</pre>" end
    sout = string.gsub(sout, "<tt>", "")
    sout = string.gsub(sout, "</tt>", "")
    return "<table class='source'><tr><td>" .. sout .. "</table>"
end

local function ASSUME_FLAGS( tbl, lookup )
    --
    ASSUME( tbl )
    lookup= lookup or {}

    -- Make sure 'tbl' only has the named tags, no else.
    --
    -- Note: prefixing flags with '_' can be used to disable them (no complaints)
    --    
    for k,v in tbl do
        if string.sub(k,1,1)=='_' then
            -- '_xxx'; no complaints
        elseif not lookup[k] then
            error( "Unexpected param: "..k.. (v~=nil and ("="..tostring(v)) or "") )
        end
    end
end

-----
-- void= CHECK_IM_VERSION()
--
-- ImageMagick detection
--
local has_im    -- 'true' when tested

local function CHECK_IM_VERSION( match_str )
    --
    if has_im then return true end   -- tested already
    
    -- 'support.lua' has an enhanced 'os.execute' for us.. (with catching)
    --
    local rc,sout,serr= os.execute( "convert -version", 3 )

    if rc~=0 or (not sout) then
        error "*** ImageMagick (v.6.x) required ***"
    end

    -- Sample output (OS X):
    --      "Version: ImageMagick 5.5.6 04/01/03 Q16 http://www.imagemagick.org
    --       Copyright: Copyright (C) 2003 ImageMagick Studio LLC"
    --
    local ver= third( string.find( sout, "ImageMagick ([%d%.]+) " ) )
    --
    -- "5.5.6"

    local _,_, a,b,_= string.find( ver, "(%d+)%.(%d+)" )
    ASSUME(a)
    
    if tonumber(a)<6 then
        error( "*** ImageMagick v.6.x required *** ("..ver.." found)" )
    end
    
    return true -- all ok :)
end

-----
-- fn_str= Loc_OutName( fn_str / sfx_str )
--
-- Returns a unique filename under the "temp/" subdirectory.
--
local tmpname_count= 0

local function Loc_OutName( str )
    --
    str= string.gsub( str, "^%*", "" )  -- "*.sfx" same as ".sfx"
        
    if string.sub(str,1,1)=='.' then    -- just suffix, generate filename
        --
        tmpname_count= tmpname_count+1
        str= "file"..tmpname_count..str  -- keep suffix
    end
   
    local path,fn= PathSplit( CurrentFile(), true )
   
    -- We can make a subdir per each HTML file, or just use naming to keep them separate.
    -- 
    if false then
        -- subdir per each source file
        path= "temp/"..(path or "")..fn..'/'
        fn= str
    else
        -- avoid making lots of subdirs
        
        if fn=="index" then   -- special case for shorter filenames (still remains unique)
            fn= str
        else
            fn= fn..'-'..str    
        end

        fn= string.gsub( path, "[/\\]", "-" )..fn
        path= "temp/"   -- place all temp files in one dir
    end
        
    Mkdir( path )   -- make sure the target subdir is there

    return path..fn
end


-----
-- fn_str= Loc_Convert( convert_str )
--
-- Interface to powerful ImageMagick picture processing.
--
-- Returns: the (temporary) output filename.
--
local function Loc_Convert( convert )
    --
    CHECK_IM_VERSION()   -- 6.x required

    convert= string.gsub( convert, "%s+", " " )   -- clean multiple spaces (may contain newlines)
    
    convert= string.gsub( convert, "([!%(%)])", "\\%1" )    -- chars needing an escape (for shell)
    
    local fn_out
    local head, last_word= skip2( string.find( convert, "(.+)%s+(%S+)$" ) )

    -- is last word a filename (suffix, at least) but not a "tile:blahblah" kindof param:
    --    
    if last_word and string.find(last_word,"%.") and
       (not string.find(last_word,":")) then
        --
        fn_out= Loc_OutName(last_word)    -- file type or full name given
        convert= head
    else
        fn_out= Loc_OutName(".jpg")    -- default file type
    end

    local path= CurrentDir()    -- subpath of current file
    local up_path= ""
    
    if path then
        up_path= string.rep( "../", second( string.gsub(path,"[/\\]","") ) )
    end
    
    local cmd= (path and ("cd "..string.sub(path,1,-2).."; ") or "").."convert "..convert..' '..up_path..fn_out
    io.stderr:write(cmd..'\n')

    local rc= os.execute( cmd )
    if rc~=0 then
        SYNTAX_ERROR "bad 'convert' syntax"    -- we know IM 6.x is installed (couldn't be that)
    end

    ASSUME( FileExists( fn_out ) )

    return up_path..fn_out
end

--=== Filter functions (global) ===--

-----
-- <@DOCUMENT [title=str] [author=str] [keywords=str] [stylesheet=url] 
--            [background=url/#hex/colorname]>
--
function DOCUMENT( tbl )
    --
    ASSUME_FLAGS( tbl, {title=1,author=1,keywords=1,stylesheet=1,background=1} )

    local str= "<html><head>\n"
    
    if tbl.title then
        str= str..[[
<title>]]..(tbl.title)..[[</title>
]]
    end

    if tbl.author then
        str= str..[[
<meta name="Author" content="]]..(tbl.author)..[[">
]]
    end

    if tbl.keywords then
        str= str..[[
<meta name="Keywords" content="]]..(tbl.keywords)..[[">
]]
    end
    
    if tbl.stylesheet then
        str= str..[[
<link rel="stylesheet" type="text/css" href="]]..(tbl.stylesheet)..[[">
]]
    end
    
    local bg= tbl.background
    if bg then
        if string.find( bg, "%." ) then     -- picture file
            bg= " background=\""..bg.."\""
        else
            bg= " bgcolor=\""..bg.."\""   -- "#ffffff", "red" etc.
        end
    end
    
    str= str..[[
</head>
<body]]..(bg or "")..'>'
    
    return str
end

-----
-- <@EVAL>lua expession</@EVAL>
--
function EVAL( tbl, text )
    --
    ASSUME_FLAGS(tbl,nil)
    
    if not text then
        return ""   -- nothing to do!
    else
        return loadstring( "return "..text )()
    end
end

-----
-- <@FONT [size_int] ["i"/"italic"] ["u"/"underlined"] ["red"] ...>text</@>
--
-- Normal <font> doesn't allow italics, does it..?  Frustrating!
--
-- With this func, all settings can be given at once, and the terminating tag
-- closes them.
--

-- There's >600 of these, we probably won't list them all.. :)
--
local color_lookup= { red=1, blue=1, yellow=1, white=1, black=1, green=1, gray=1 }

function FONT( tbl, text )
    --
    local size, italic, underlined, bold, color
    
    for k,_ in tbl do
        if tonumber(k) then
            size= tonumber(k)   -- -1/0/1
            --
        elseif k=="italic" or k=="i" then
            italic= true
        elseif string.find(k,"under") or k=="u" then
            underlined= true
        elseif k=="bold" or k=="b" then
            bold= true
        elseif color_lookup[k] then
            color= k    -- "red", ...
        else
            SYNTAX_ERROR( "Unknown @FONT param: "..k.."= "..tostring(v) )
        end
    end

    local pre= "<font"..
                    ((size and ' size="'..size..'"') or "")..
                    ((color and ' color="'..color..'"') or "")..
                    ">"
    local post= "</font>"
    
    if italic then
        pre= pre.."<i>"
        post= "</i>"..post
    end                    
    if underlined then
        pre= pre.."<u>"
        post= "</u>"..post
    end                    
    if bold then
        pre= pre.."<b>"
        post= "</b>"..post
    end                    

    return pre..text..post
end

-----
-- <@FUNC>lua code</@FUNC>
--
-- Note: This tag may be replaced by '<@LUA>' - or not..?-)
--
function FUNC( tbl, text )
    --
    ASSUME_FLAGS(tbl,nil)
    ASSUME(text, "@FUNC without ending tag! (empty lines not allowed)")
    
    local block= ASSUME( loadstring( "function "..text ), "Syntax error in Lua code" )

    block()    -- add the new tags :)
    
    return "<!--"..text.."-->"   -- HTML output
end

-----
-- <@GRA src=url [align=right/left/center] [border[=N]] [x=width] [y=width]
--               [spacing=N] [padding=N] [tab=N]
--               [link[=url] [popup]]
--               [convert=str] [convert_to="gif"/"jpg"/"png"/...]>
--    [..picture text..
-- </@GRA>]
--
function GRA( tbl, text )
    --
    ASSUME_FLAGS( tbl, {align=1,src=1,x=1,y=1,link=1,popup=1,border=1,spacing=1,padding=1,tab=1,
                        convert=1,convert_to=1} )

    ASSUME( tbl.src or tbl.convert, "<@GRA> must have 'src' or 'convert'" )
    
    local src= tbl.src
    local align= tbl.align
    local x= tbl.x
    local y= tbl.y
    local link= (tbl.link==true) and src or tbl.link
    local popup= link and tbl.popup
    
    local border= ((tbl.border==true) and 1) or tbl.border
    local spacing= tbl.spacing
    local padding= tbl.padding

    local tab= tbl.tab
    local convert= tbl.convert
    local convert_to= tbl.convert_to

    ASSUME( not convert_to )    -- depricated!
    
    -- Doing a conversion (with ImageMagick) uses another picture file; no other change.
    --
    if convert or convert_to then
        --
        if src then
            convert= src..' '..convert
        end
        src= Loc_Convert( convert )    -- that's it!
    end
    
    -- Always put the pic within a table (allows the picture text to be beneath)
    --
    local str="<table"..  -- border="..border..
              (align and " align="..align or "")..
              "><tr>"..
              (tab and ("<td width="..(tab)..">") or "")..  -- tabulator (left margin)
              "<td>"

    if border or spacing or padding then
        str= str.."<table"..
                    ((spacing and " cellspacing="..spacing) or "")..
                    ((padding and " cellpadding="..padding) or "")..
                    ((border and " border="..border) or "")..
                  "><tr><td>"
    end
    
    if link then 
        str= str.."<a href=\""..link.."\""   -- lead further (to pic itself)
        if popup then str= str.." target=\"_blank\"" end
        str= str..">"
    end
    
    str= str.."<img src=\""..src.."\" align=center"
        if x then str= str.." width="..x end
        if y then str= str.." height="..y end
        str= str..">"   -- <img..>
    
    if link then
        str= str.."</a>"
    end    

    if border or spacing or padding then
        str= str.."</tr></table>"   -- inner table (around the pic)
    end
    
    if text then
        str= str.."</tr><tr><td><p align=center>"..
                  "<font face=Arial size=-2><i>"..text.."</i></font>"
    end
    
    str= str.."</tr></table>"
    
    return str
end

-----
-- <@INCLUDE url=filename/url [expand=false]>
--
function INCLUDE( tbl, text )
    --
    ASSUME_FLAGS( tbl, {url=1,expand=1} )
    ASSUME( not text )

    ASSUME( ReadFile )  -- should be in our disposal (from Lumikki 'dirtools.lua')
    
    local url= ASSUME( tbl.url )
    
    local str= ReadFile( url )
    ASSUME( str, "Unable to read: "..url )
    
    return str, (tbl.expand~=false)
end

-----
-- <@LINK [popup[=str]] [url=str]>..text..</@LINK>
--
function LINK( tbl, text )
    --
    --ASSUME_FLAGS( tbl, {popup=1,url=1} )
    ASSUME( text )
    
    local popup, url
    
    for k,v in tbl do
        --
        if k=="popup" then
            popup= (v==true and "_blank") or v
        elseif k=="url" then
            ASSUME( not url )
            url= v
        else    -- free url's (skipping "url=")
            ASSUME( not url )
            ASSUME( v==true )   -- no value
            url= k
        end
    end

    if not url then
        url= text
    end
    
    if string.find( url, "^(%a+)://" ) or   -- "http://", "ftp://" etc.
       string.find( url, "^%.%./" ) or      -- "../", definately relative
       string.find( url, "^mailto:" ) then
        --
    elseif string.find( url, "@" ) then    -- shortcut "me@somewhere.net"
        url= "mailto:"..url
        --
    elseif string.find( url, "^www%." ) then
        url= "http://"..url
        --
    elseif string.find( url, "^ftp%." ) then
        url= "ftp://"..url
        --
    else
        -- If the string has i.e. "luaforge.net/..." add the "http://" prefix,
        -- otherwise treat it as relative.
        --
        local tmp= third( string.find( url, "(.-)/" ) )
        
        if tmp then
            if string.find(tmp,"%.") then   -- first part has a dot; seems like a site
                url= "http://"..url
            end
        else
            -- No slash - if it's ".org", ".net" etc.. it's a site.
            --            if it's ".htm(l)", ".jpg", ".gif", ... it's a file.
            --
            local sfx= string.lower( third( string.find( url, "(%..-)$" ) ) )
            
            if ({ org=1, net=1 })[sfx] then
                url= "http://"..url
            end
        end
    end

    local str= "<a href=\""..url.."\""..
               (popup and " target=\""..popup.."\"" or "")..
               ">"
    return str..text.."</a>"
end

-----
-- <@LUA>lua code</@LUA>
--
-- Similar to <@FUNC> but requires 'function' word.  Suits better to 
-- definition of multiple functions at a time.
--
function LUA( tbl, text )
    --
    ASSUME_FLAGS(tbl,nil)
    ASSUME(text, "@LUA without ending tag! (use '<\\@' for extended tags within the code)")

    -- Remove HTML comments if any..
    --
    text= string.gsub( text, "<!%-%-.-%-%->", "" )
    
    -- Enable escaped Lumikki tags:
    --
    text= string.gsub( text, "<\\@", "<@" )
    
    local block= ASSUME( loadstring( text ), "Syntax error in Lua code" )

    block()
    
    return ""   -- HTML output
end

-----
-- <@SKYPE me=str>
--
-- Skype Me (www.skype.com) logo, with your caller id attached.
--
function SKYPE( tbl, text )
    --
    ASSUME_FLAGS( tbl, {me=1} )
    ASSUME( not text )
    
    local me= tbl.me
    if not me then
        SYNTAX_ERROR( "SkypeMe requires id!" )
    end

    return [[<a href="callto://]]..me..[["><img src="http://goodies.skype.com/graphics/skypeme_btn_red.gif" border="0" alt="Skype Me!"></a>]]
end

-----
-- <@SMILEY [type=str]>
--
local smiley_lookup= {
    [":)"]= "http://www.ous.edu/images/smiley.gif",
    [";P"]= "http://forums.invisionpower.com/html/avatars/Smiley_Avatars/smiley-grimmace.gif"
    --...
    }

function SMILEY( tbl )   -- ;)
    --
    ASSUME_FLAGS( tbl, {type=1} )

    local grin= tbl.type or ":)"
    local url= smiley_lookup[grin]
    
    if not url then     -- text
        return grin
    else
        return [[<img src="]]..url..[[" alt="]]..grin..[[">]]
    end
end

--[[
-----
-- <@TAB int>
--
-- Tabulator, sort of.. :)
--
function TAB( tbl, text )
    --
    ASSUME( not text )
    
    for k,_ in tbl do
        return string.rep( "&nbsp;", k )
    end
end
]]--

-----
-- <@TAG tagname>
--
-- This func allows tags to be written in HTML code without the clumsy &lt;TAG&gt;
-- notation. The syntax is somewhat special, allowing any name to be used within
-- the tag (but that makes it short :).
--
function TAG( tbl, text )
    --
    ASSUME( not text, "Old syntax not supported: use <@TAG tagname>" )
    
    for k,_ in tbl do
        return "&lt;"..k.."&gt;"
    end
end

--
-- LUMIKKI.LUA                 -- Copyright (c) 2004, asko.kauppi@sci.fi
--
-- Lua website generation tool.
--
-- This library allows generation & maintenance of full, static HTML websites
-- based on Lua code, with automatic navigation links etc.  
--
-- The tool can also be used stand-alone (see manual.lhtml) as a filter to 
-- generate individual HTML pages, a bit like TeX, but with Lua syntax. :)
--
-- The manual of this tool (manual.lhtml) will also work as its sample.
--
-- Lua 5.0 is required (or any 100% compatible derivative).
--
-- Usage:
--      lua [-lmydef.lua] lumikki.lua input.lhtml [...] >output.htm
--      lua [-lmydef.lua] lumikki.lua dirname/.   (NOT IMPLEMENTED, YET!)
--
--      The first line processes a single file (any filename suffix will do),
--      the second line processes a whole directory tree (*.lhtml -> *.html).
--
--      Use of the standard '-l' Lua command line flag allows feeding in your
--      custom function definitions for running Lumikki.
--
-- License: GPL (see license.txt)
--
--      This program is free software; you can redistribute it and/or modify
--      it under the terms of the GNU General Public License as published by
--      the Free Software Foundation; either version 2 of the License, or
--      (at your option) any later version.
--
--      This program is distributed in the hope that it will be useful,
--      but WITHOUT ANY WARRANTY; without even the implied warranty of
--      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--      GNU General Public License for more details.
--
--      You should have received a copy of the GNU General Public License
--      along with this program; if not, write to the Free Software
--      Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
--
-- Using the GPL license does _not_ restrict you from using this tool in
-- building commercial, closed source applications. Regard it the same as 
-- the gcc compiler - if you just use the tool and don't change it, you're fine.
-- If you change it, share that change with others. :)
--
-- Author(s):
--      asko.kauppi@sci.fi
--
-- To-do:   ..
--
-- History:
--
-- 14-Oct-04/ak: Tested with Gentoo Linux, NetBSD as well. Removed any OS X specifics;
--               all non-Win32 systems now dealt with similarily (generic Posix).
-- 7-Sep-04/ak:  Started a revise round, adding handling of full directory trees.
--               Split the code into pieces.
-- 30-Jun-04/ak: v.0.04 added @GRA tag.
-- 27-Jun-04/ak: Allowed use of HTML comments with empty lines, and skipping
--               extended tags within such comments (may be handy).
-- 24-Jun-04/ak: v.0.03 added @FUNC and @LUA tags for embedding Lua code.
--               Revised the documentation (all funcs now mentioned).
-- 23-Jun-04/ak: v.0.01 ready and working. Oh boy! :)
-- 16-Jun-04/ak: FINALLY, started the work for real.. (after much todo..)
--
local mymod= { _info= { MODULE= "HTML site generator",
                    AUTHOR= "asko.kauppi@sci.fi",
                    COPYRIGHT= "Copyright (c) 2004, Asko Kauppi",
                    RELEASE= 20041014,  -- release date (version)
                    LICENSE= "GPL" } }
local tmp,_

ASSUME= ASSUME or assert

--
local m= ( function()

--
-- GLOBALS.LUA
--
-- Purpose: Preventing unintended use of global variables.
--
-- Note:    You can always use 'rawset' and 'rawget' to get direct access that
--          bypasses this safety net (not recommended for applications).
--
local m= { _info= { MODULE= "Catching unintended globals use",
                    AUTHOR= "asko.kauppi@sci.fi",
                    LICENSE= "LGPL" } }


local default_declares=  -- allow some names to be used, even nondeclared
        { ['_']= 1,  -- use of '_' as nondeclared, temporary variable 
          arg= 1,    -- arguments to a file
          _ALERT= 1, -- used by 'lua_error()'
        }

--
local function Loc_PrintCatch( reason, index )
    --
    io.stderr:write( "\n\t*** "..reason.." '" ..index.. "' ***\n\n" )

    --print( debug.traceback() )

    local info= debug.getinfo(3)
    --
    -- .source  lua filename
    -- .what    "main"
    -- .func    function object
    -- .short_src lua filename
    -- .currentline 19
    -- .namewhat ""
    
    local src= assert( info.short_src )
    local what= assert( info.what )
    local line= assert( info.currentline )
    
    io.stderr:write( "\tAt: "..src.." line "..line )

    if what~="main" then 
        io.stderr:write( " ("..what..")" )
    end
    io.stderr:write "\n"
    
    -- Note: 'assert(false)' would not break outside scripts (using 'pcall').
    --       'os.exit()' exits all at once.  
    --
    os.exit(-1)
end

--
local function Loc_SettableEvent( tbl, index, value )
    --
    if (type(value)~="function") and (not m._declared[ index ]) then
        --
        Loc_PrintCatch( "Undeclared global:", index )
        ASSUME(nil)
    end
    
    rawset( tbl, index, value )
end

--
local function Loc_GettableEvent( tbl, index )
    --
    if not m._declared[ index ] then
        Loc_PrintCatch( "Reading undeclared global:", index )
        ASSUME(nil)
    end
    
    return rawget( tbl, index )
end


-----
-- Declare globals (so that we can initialise them & use even if they're 'nil')
--
-- Note: Tables are tolerated (flattened) so that you can enter the list of
--       global names either as:
--          declare 'a'
--          declare( 'a','b','c' )
--          declare{ 'a','b','c', {'d','e'} }
--
function m.declare( ... )
    --
    for i=1, table.getn(arg) do
        --
        local v= arg[i]
        local t= type(v)
        
        if t=="table" then
            m.declare( unpack(v) )     -- flatten recursively
            --
        elseif t=="string" then
            m._declared[v]= 1    -- now known in the lookup
        else
            error( "Unable to declare variable '"..(v or "nil").."'!" )
        end
    end
end

function m.reset()
    --
    m._declared= default_declares
end

-----
-- List globals (on the screen):
--
function m.list()
    --
    io.stderr:write "Declared globals:\n"
    
    for k,_ in (m._declared) do
        io.stderr:write( "\t"..k..'\n' )
    end
end

--
function m.on()
    --
    local mt
    local globals= getfenv(0)

    mt= getmetatable(globals)
    
    if not mt then
        mt= {}
        setmetatable( globals, mt )
    end
    
    mt.__newindex= Loc_SettableEvent
    mt.__index= Loc_GettableEvent
end

--
function m.off()
    --
    local mt
    local globals= getfenv(0)

    mt= getmetatable(globals)
    
    if not mt then
        mt= {}
        setmetatable( globals, mt )
    end
    
    mt.__newindex= nil
    mt.__index= nil
end

--
function m.reset()
    m._declared= default_declares  -- lookup of globals (names as indices)
    m.on()
end

--
function m.help()
    --
    print "\n"
    print "Undeclared use of global variables will now be trapped."
    print ""
    print "\t\"globals.declare 'a'"
    print "\t\"globals.declare{'a','b','c'}\" declares globals for use."
    print "\t\"globals.reset()\" resets the declarations."
    print "\t\"globals.list()\" lists your globals."
end

-----
-- Initialisation
--
if not m._declared then   -- first time
    m.reset()
    --m.help()
end

return m
end )()

m.on()
--foo= 22     -- gotcha!

--
do    -- keep support's local stuff up to itself

--
-- SUPPORT.LUA
--
-- Helpful tools and standard library overrides/fixes/extensions..
--
-- AK(14-Oct-2004): Removed LINUX & NETBSD checks (unnecessary, right?)
--




rawset( _G, "WIN32", os.getenv("WINDIR") ~= nil )   -- otherwise Posix

-----
-- void= dump( tbl [,lev] )
--
function dump( tbl, lev )
    --
    lev= lev or 0

    if not tbl then
        io.stderr:write "(nil)\n"
        return
        --
    elseif type(tbl)=="table" then
        --
        local pre, pre2
        local items= false

        pre= string.rep("  ", lev)

        for key,val in tbl do
            --
            items= true

            pre2= pre..key.." : "

            if type(val)=='table' then
                io.stderr:write( pre2.."table\n" )
                if lev >= 0 then  -- recurse
                    dump( val, lev+1 )
                end
            elseif type(val)=='string' then
                io.stderr:write( pre2.."\t'"..val.."'\n" )
            else
                io.stderr:write( pre2,val..'\n' )
            end
        end

        if not items then
            io.stderr:write( pre.."(empty)\n" )
        end
    else
        io.stderr:write( "("..type(tbl)..") ".. tbl..'\n' )
    end
end

-- Debugging support:  TRACE(true), TRACE(false)
--
-- reason: "call"/"return"/"tail return"/"line"/"count"
--
local function my_callhook( reason )    -- "call"
    --
    local info= debug.getinfo( 2 )
    --
    -- .source      '@Scripts/globals.lua'
    -- .what        'Lua'/'C'/'main'
    -- .func        function
    -- .short_src   'Scripts/globals.lua'
    -- .currentline -1
    -- .namewhat    ''
    -- .linedefined 39
    -- .nups        2

    local name= (info.name) or ""
    local str= " -> "..(info.source).." "..(info.linedefined).." "..name
    
    io.stderr:write(str..'\n')
end

function TRACE( on_off )
    --
    if not on_off then
        debug.sethook( nil )
    else
        debug.sethook( my_callhook, "c" )
        --
        -- "c": Lua calls a function
        -- "r": Lua returns from a function
        -- "l": for each line
    end
end

-----
-- Credit: Thanks to Jamie Webb for the trick with the '_' arguments.
--
function first(x) return x end
function second(_,x) return x end
function third(_,_,x) return x end

ASSUME( first(1,2,3,4)==1 )
ASSUME( second(1,2,3,4)==2 )
ASSUME( third(1,2,3,4)==3 )

function skip2(_,_, ...) return unpack(arg) end

-----
-- iterator_func= string.lines( str )
-- iterator_func= lines( str/file )
--
-- This allows 'for line in lines(str) do .. end' 
-- ..just as standard libs already do with 'io.lines()'
--
ASSUME( not rawget(string,"lines") )
ASSUME( not rawget(_G,"lines") )

string.lines= function(str) return string.gfind(str, "[^\n]+") end

function lines(val)     -- str/file
    return type(val)=="string" and string.lines(val)
                                or io.lines(val)
end

--selftest
local tmp=""
for l in lines( "a\nb b\nc c c\n" ) do
    --print(l)
    tmp= tmp..l..'+'
end
ASSUME( tmp == "a+b b+c c c+" )

-----
-- name_str= TempName( [dir_str [,progid_str]] )
--
local magic1= string.sub( os.time(), -4 )   -- startup time in seconds (last N digits)
local magic2= 0    -- counter

local TMP_PATH= "/tmp/"   -- default

if WIN32 then  -- get temp dir
    --
    local s= os.getenv("TEMP") or os.getenv("TMP")
    ASSUME( s, "TEMP env.dir not set!" )

    TMP_PATH= s..((string.sub(s,-1)~='\\') and '\\' or "")
end

local function Loc_TempName( dir, id )
    --
    magic2= magic2 + 1    -- times we've called this routine

    local fn= TMP_PATH..(id or "temp")..magic1..magic2..".tmp"

    -- The file should never already exist:
    --
    ASSUME( not io.open(fn,"r") )
   
    return fn
end

-----
-- rc_int [,stdout_str, stderr_str] = os.execute( cmd_str [,catch_int] )
--
-- The added 'catch_int' parameter allows transparent catching of stdout
-- and/or stderr output (1=stdout,2=stderr,3=both).
--
local _os_execute= ASSUME( os.execute )  -- original

local function Loc_ReadAllAndDump(fn)
    --
    local f= io.open(fn,"rt")
    if not f then
        return nil    -- no file, the command did not run..
    end
    
    local str= f:read'*a'   -- read all
    f:close()
    os.remove(fn)
    
    -- Remove terminating linefeed (if any) - eases one-liner analysis
    --
    if string.sub( str,-1 )=='\n' then
        str= string.sub( str, 1,-2 )
    end
    
    return str
end

function os.execute( cmd, catch, input )
    --
    if not catch then
        return _os_execute(cmd)     -- old way
    else
        local fn0= input and Loc_TempName()
        local fn1= (catch==1 or catch==3) and Loc_TempName()
        local fn2= (catch>=2) and Loc_TempName()

        if fn0 then
            local fh = io.open(fn0, "wb")
            fh:write(input)
            fh:close()
        end

        local rc= _os_execute( cmd..(fn0 and " <"..fn0 or "")
                                  ..(fn1 and " >"..fn1 or "")
                                  ..(fn2 and " 2>"..fn2 or "") )
    
        local str0= fn0 and Loc_ReadAllAndDump(fn0)
        local str1= fn1 and Loc_ReadAllAndDump(fn1)
        local str2= fn2 and Loc_ReadAllAndDump(fn2)
    
        return rc, str1,str2
    end
end

--selftest
--
local rc,stdout= os.execute( "echo AAA!", 1 )

-- Win32 seems to put an empty space after an 'echo' string.
--
ASSUME( rc==0 and (stdout=="AAA!" or (WIN32 and stdout=="AAA! ")),
        "<"..stdout..">" )

-----
-- [tbl]= table.copy( [tbl [,filter_func] )
-- [tbl]= table.icopy( [tbl [,filter_func] )
--
-- The filter func is fed each item at a time. If it returns 'nil', that
-- item will not be copied further.
--
-- Note: If 'nil's are returned, 'table.copy' leaves holes in the returned
--       table (it doesn't change keys).  Use 'icopy' if the indices don't
--       matter (it retains the order, but skips 'nilled' keys). 
--
function table.copy( tbl1, filter_func )
    --
    if not tbl1 then return nil end    -- 'nil' pass-through
    
    local tbl2= {}
    if not filter_func then     -- faster this way
        for k,v in tbl1 do tbl2[k]= v end
    else
        for k,v in tbl1 do
            local tmp= filter_func(v)
            if tmp~=nil then   -- skip if 'nil' (but passes 'false')
                tbl2[k]= tmp
            end
        end
    end

    return tbl2
end

function table.icopy( tbl1, filter_func )
    --
    if not tbl1 then return nil end    -- 'nil' pass-through
    
    local tbl2= {}
    if not filter_func then     -- faster this way
        for _,v in ipairs(tbl1) do table.insert(tbl2,v) end
    else
        for _,v in ipairs(tbl1) do
            local tmp= filter_func(v)
            if tmp~=nil then   -- skip if 'nil' (but passes 'false')
                table.insert( tbl2, tmp )
            end
        end
    end

    return tbl2
end

-----
-- n_uint= table.getn( tbl )
--
-- Problem: If there are 'holes' in a table ('nil' values), the behaviour of 'table.getn'
--          is dependent on Lua version.
--
--          Up until Lua 5.1-w1, 'table.getn()' always returned the FIRST index followed
--          by a 'nil', either before a 'hole' or genuinly at the end of the table.
--
--          With Lua 5.1-w1, 'table.getn()' sometimes does that, sometimes hops over the
--          holes. It returns SOME index followed by a 'nil'.
--
-- Solution:
--          To keep applications ignorant of the underlying Lua version, we enforce the
--          OLD way, unless Lua folks permanently change the way 'table.getn' _should_
--          work with holes.
--
local GETN_51W1= table.getn( {'a','b',[4]='c' } ) == 4   -- true for Lua5.1-w1

if GETN_51W1 then
    local _old_getn= table.getn
    --
    table.getn= 
      function( tbl )
        local n= _old_getn(tbl)    -- over the holes (or not..)
        for i=1,n do
            if tbl[i]==nil then   -- allow 'false'
                return i-1
            end
        end
        return n
      end
end

--
local ab_c= { 'a','b', [4]='c' }
ASSUME( table.getn(ab_c) == 2 )

for i,v in ipairs(ab_c) do
    ASSUME( i<=2 )
end

-----
-- switch / case construct
--
-- Credits: Eric Tetz (sent to lua-list 1-Sep-04)
--
-- Usage:
--      switch( action,
--              { DOG_BARK, do print "Arf!!!" end },
--              { DOG_BITE, do print "Chomp!" end },
--              { DOG_SLEEP, do print "Zzzzzz..." end },
--              { nil, do print "Default!" end }
--            )
--
-- Note: The 'key' used may be of any Lua data type.
--       A table key means any of its members will do.
--
ASSUME( (not rawget(_G,"switch") ) ) 
ASSUME( (not rawget(_G,"case") ) )
ASSUME( (not rawget(_G,"default") ) )

function switch( key, ... )
    --
    local match= false
    
    for _,item in ipairs(arg) do
        --
        ASSUME( type(item)=="table" )
        
        if type(item[1])=="table" then  -- multiple keys
            for _,vv in item[1] do
                if vv==key then
                    match=true; break
                end
            end
        else
            if (item[1]==nil) or (item[1]==key) then
                match= true
            end
        end
        
        if match then
            local v= item[2]
            
            if type(v)=="function" then
                return v()
            else
                return v  -- string or whatever
            end
        end
    end
end

function case( key, v )
    return { key, v }
end

function deafult( v )
    return { nil, v }
end


end

--
do

--
-- DIRTOOLS.LUA
--

-----
-- bool= FileExists( fn_str )
--
local function Loc_FileExists( fn )
    --
    local f= io.open(fn,'r')
    if f then
        f:close()
        return true
    else
        return false
    end
end

-----
-- path_str, filename_str [,suffix_str]= PathSplit( path_and_filename [,split_sfx_bool] )
--
local function Loc_PathSplit( str, split_sfx )
    --
    local path,fn,suffix
    
    if not str then return nil end

    _,_,path,fn= string.find( str, "(.*[/\\])(.*)" )

    if path then
        if fn=="" then fn=nil end
    else
        path= nil
        fn= str     -- just filename
    end
    
    if split_sfx then
        local _,_, s1,s2= string.find( fn, "(.+)%.(.-)$" )
        if s2 then
            fn= s1
            suffix= s2
        end
    end
    
    return path,fn,suffix
end

-----
-- bool= IsSlashTerminated( str )
-- str= MakeSlashTerminated( str )
--
local function Loc_IsSlashTerminated( str )
    --
    local c= string.sub( str, -1 )
    return ((c=='/') or (c=='\\'))
end

local function Loc_MakeSlashTerminated( str )
    --
    return Loc_IsSlashTerminated(str) and str or str..'/'
end

-----
-- str= QuoteIfSpaces(str)
--
-- Add quotes so that a file or pathname (with spaces) can be used in OS commands.
--
local function Loc_QuoteIfSpaces( str )
    --
    return string.find( str, ' ' ) and ('"'..str..'"') or str
end

-----
-- void= ParentDir( dir_name )
--
-- Returns name of the parent or 'nil' for root level.
--
local function Loc_ParentDir( str )
    --
    local daddy
    
    -- Remove the terminating slash, see if there's any other left:
    --
    ASSUME( Loc_IsSlashTerminated(str) )
    daddy= first( Loc_PathSplit( string.sub( str, 1,-2 ) ) )
    
    if WIN32 and daddy then     -- special root detection
        if string.find( daddy, ".%:$" ) then
            return nil  -- just drive name
        end
    end

    return daddy    -- 'nil' for root
end

-----
-- bool= DirProbe( dir_name )
--
-- Checks whether we can write to a directory.
--
local function Loc_DirProbe( dirname )
    --
    ASSUME( Loc_IsSlashTerminated(dirname) )

    local fn= dirname.."~probe.tmp"
    
    -- overwrite if file is already there (should be okay)
    --
    local fh= io.open( fn, "wt" )
    if not fh then
        return false    -- did not exist (or not write access)
    end

    fh:close()
    os.remove( fn )
    
    return true     -- dir existed
end

-----
-- void= Mkdir( dir_name )
--
-- Make sure that the target directory exists.
--
local function Loc_Mkdir( dirname )
    --
    if not dirname then return end   -- root met

    ASSUME( Loc_IsSlashTerminated(dirname) )

    if WIN32 then
        dirname= string.gsub( dirname, '/', '\\' )
    end

    if not Loc_DirProbe( dirname ) then
        --
        Loc_Mkdir( Loc_ParentDir(dirname) )
        local rc= os.execute( "mkdir ".. Loc_QuoteIfSpaces(dirname) )
        
        ASSUME( rc==0, "Unable to create '"..dirname.."'!" )
        
        ASSUME( Loc_DirProbe( dirname ) )  -- should be there now
    end
end

-----
-- tbl= Loc_DirCmd( cmd_str )
--
local function Loc_DirCmd( cmd )
    --
    local rc,str= os.execute( cmd, 1 )  -- catch stdout

    if rc~=0 then
        return {}
    end

    local ret= {}
    
    for l in string.gfind(str,"[^\n]+") do
        table.insert(ret,l)
    end
    return ret
end

-----
-- [tbl]= Loc_DirList( dirname [,dirs_only_bool] )
--
-- Returns a list of the contents of a directory (or 'nil' if no such dir exists).
-- Subdirectory entries are marked with a terminating slash ('/').
--
-- Note: For consistancy (and portability of applications) even WIN32
--       paths are returned using forward slashes ('/').
--
local function Loc_DirList( dirname, dirs_only )
    --
    dirname= dirname and Loc_MakeSlashTerminated(dirname) or ""
    if dirname=="./" then dirname= nil end

    local ret= {}

    if WIN32 then
        --
        -- "/AD" limits to directories only
        -- "/B" causes no headers & footers, just filenames (is it already in Win95?)
        --
        local lookup= {}

        dirname= Loc_QuoteIfSpaces( string.gsub( dirname, '/', '\\' ) )
        
        local tbl= Loc_DirCmd( "dir /AD /B "..dirname )   -- only dirs

        -- Add slash to every dir line
        --
        for i,v in ipairs(tbl) do
            table.insert( ret, v..'\\' )
            lookup[v]= true
        end
        
        if not dirs_only then   -- Add non-dirs, too?
            --
            tbl= Loc_DirCmd( "dir /B "..dirname )   -- dirs & files

            for i,v in ipairs(tbl) do
                if not lookup[v] then   -- not in the dirs table
                    table.insert( ret, v )
                end
            end            
        end
        
        -- Return with forward slashes
        --
        if true then
            for i=1,table.getn(ret) do
                ret[i]= string.gsub( ret[i], '\\', '/' )
            end
        end
        --
    else    -- Linux, OS X, BSD, Win32/MSYS...
        --    
        -- "-1" causes all output in a single column
        -- "-F" adds slash after directory names (and some more postfixes that
        --      we might need to filter away)
        --
        ret= table.icopy( Loc_DirCmd( "ls -1 -F ".. Loc_QuoteIfSpaces(dirname) ),
                function(v)   -- filter
                    local c= string.sub(v,-1)   -- last char
                    if v=='./' or v=='../' then -- QNX has these
                        return nil
                    elseif c=='/' then
                        return v    -- dirs are always welcome :)
                    elseif dirs_only then
                        return nil  -- skip non-dir entries away
                    elseif c=='*' then  -- executables
                        return string.sub( v, 1,-2 )
                    elseif c=='@' then  -- symbolic links
                        return nil
                    else
                        return v
                    end
                end )
    end

    return ret
end

--
local function Loc_LinesToTable( str )
    --
    local ret= {}
    
    -- "" -> { "" }
    -- nil -> {}
    --
    for l in string.lines(str) do
        table.insert( ret, l )
    end
    return ret
end

-----
-- [bool=] FileIsNewer( fn1_str, fn2_str )
--
-- Returns 'true' if 'fn1' is newer than (has been modified after) 'fn2'.
--
-- Note: While 'ls' does not show file times by the seconds, we can still use its
--       sorting features to see which file is newer (and, well, that's all we want!)
--
local function Loc_FileIsNewer( fn1, fn2 )
    --
    if WIN32 then
        error "Not for Win32, yet!"    -- to be done!!
    else
        -- Filenames only, one per line, youngest first:
        --
        local cmd= "ls -1 -t "..Loc_QuoteIfSpaces(fn1)..' '..Loc_QuoteIfSpaces(fn2)
        
        local rc,sout= os.execute( cmd, 1 )
        ASSUME( rc==0 )

        -- Output is one or two lines ('fn2' does not need to exist).
        --
        local tbl= Loc_LinesToTable( sout )
        
        ASSUME( tbl[1] )    -- should have at least 'fn1'
        ASSUME( not tbl[3] )    -- what!?
        
        if tbl[1]==fn1 then
            return true   -- fn1 was youngest (or only)
        else
            ASSUME( tbl[1]==fn2 )
            ASSUME( tbl[2]==fn1 )
            return false  -- fn1 was older
        end
    end
end

-----
-- [md5_str]= MD5( fn_str )
--
-- Return the MD5 checksum for a file.
--
-- Requires: 'md5' program needs to be available (built-in on OS X).
--
local function Loc_MD5( fn )
    --
    local cmd= "md5 "..Loc_QuoteIfSpaces(fn)
    
    local rc,sout= os.execute( cmd, 1 )   -- catch output
    
    if rc~=0 then
        error "Unable to find 'md5' program!  (buhuuuuuu....)"
    end

    --[iMac:~/www.sci.fi] asko% md5 refresh.lua 
    -- "MD5 (refresh.lua) = e7d6f8e60baa831853d23de6bfd0b477"
    --
    local str= string.find( sout, "MD5 %(.+%) = (%x+)" )
    
    ASSUME( str, "Bad MD5 output: "..sout )

    return str  -- tonumber( str, 16 )
end


-----
-- void= CreateFile( fn_str, contents_str [,chmod_str] )
--
local function Loc_CreateFile( fn, str )
    --
    local fh= io.open( fn, "wt" )
    ASSUME( fh, "Unable to create '"..fn.."'!" )
      
    fh:write( str )
    fh:close()
end

-----
-- [str]= ReadFile( fn_str )
--
local function Loc_ReadFile( fn )
    --
    local f= io.open( fn, "rt" )
    if f then
        local str= f:read'*a'   -- all
        f:close()
        return str
    end
    return nil    
end

-- Declare these as global so they may be used by filters, too:
--
FileExists= assert( Loc_FileExists )
PathSplit=  assert( Loc_PathSplit )
CreateFile= assert( Loc_CreateFile )
ReadFile=   assert( Loc_ReadFile )
Mkdir=      assert( Loc_Mkdir )

PathOnly=   function(str) return first( PathSplit(str) ) end
FilenameOnly= function(str) return second( PathSplit(str) ) end

--...

end


-----===== MAIN PART =====-----
--

-----
-- str= Loc_SkipWhitespace(str)
--
local function Loc_SkipWhitespace(str)
    --
    if not str then return nil end
    return string.gsub( str, "^%s+", "" )   -- that's it!
end

ASSUME( Loc_SkipWhitespace("abc") == "abc" )
ASSUME( Loc_SkipWhitespace("   abc") == "abc" )
ASSUME( Loc_SkipWhitespace("   ") == "" )

-----
-- Use this when lhtml source has problems (shows file & line to the user)
--
-- Note: Also filter funcs can (and should) use it!
--
local current_line, current_file 

function SYNTAX_ERROR( str, line, fname )
    --
    error( "\n\n*** ERROR in '"..( (fname or current_file) or "??")..
                               "' line "..( (line or current_line) or "??")..
                               ": ".. str.."\n" )
end

-----
-- word_str [,tail_str]= Loc_WordSplit(str)
--
-- Cuts a string into first word, either at white space or '='.
--
local function Loc_WordSplit( str )
    --
    local head,sep,tail
    
    if string.sub(str,1,1)=='"' then    -- "many words" 
        --
        local n=2
        while true do
            local c= string.sub(str,n,n)
            
            if c=='\\' then
                n= n+2  -- jump over next (i.e. '\"')
            elseif c=='"' then
                break
            elseif c then
                n= n+1
            else
                SYNTAX_ERROR( "Quote mismatch!", current_line, current_file )
            end
        end

        head= string.gsub( string.sub(str,2,n-1), "\\\"", '"' )
        -- skip 'n' (it's the double hyphen)
        tail= Loc_SkipWhitespace( string.sub(str,n+1) )
    else
        head,sep,tail= skip2( string.find( str, "(.-)%s*([%s=])%s*(.+)" ) )
    
        if not head then
            head,tail= str,nil  -- no spaces
            --
        elseif sep=='=' then
            tail= sep..tail   -- white space removed, but keep the '='
        end
    end

    if tail=="" then tail=nil end
    
    if tail then
        ASSUME( not string.find( tail, "^%s" ) )    -- all white space eaten?
    end
    
    return head,tail
end

-----
-- tag_str, params_tbl= Loc_ParseTag( tag_and_params_str )
--
local function Loc_ParseTag( str )
    --
    local params= {}
    local tag,tail= Loc_WordSplit( str )

    while tail do    -- "param1[=val2] [...]"
        --
        local key,tail2= Loc_WordSplit( tail )
        local val

        if not tail2 then   -- just the "param"
            val= true
            tail= nil
            --
        elseif string.sub(tail2,1,1)=='=' then   -- "param=..."
            --
            val,tail= Loc_WordSplit( Loc_SkipWhitespace( string.sub(tail2,2) ) )
            
            if val=="true" or val=="yes" then
                val= true
            elseif val=="false" or val=="no" then
                val= false
            elseif tonumber(val) then   -- numeric?
                val= tonumber(val)
            end
        else    -- "param1 param2.."
            val= true
            tail= tail2
        end
        
        params[key]= val
    end        
    
    return tag,params
end

-----
-- start_n, stop_n= 
--
local function Loc_NextComment( str )
    --
    local n1,n2= string.find( str, "<!%-%-.-%-%->" )

    if n1 then
        return n1, n2     -- first and last index of comment        
    else
        return nil  -- no comment found    
    end
end

-----
-- head,tag,tail=
--
local function Loc_NextFuncTag( str )
    --
    local head,tag,tail= skip2( string.find( str, "(.-)<(@.-)>(.*)" ) )

    if head then
        -- Do we have a comment starting before?
        --
        local n,m= Loc_NextComment( str )

        if n and (n < string.len(head)) then
            head,tag,tail= Loc_NextFuncTag( string.sub( str,m+1 ) )
            
            if head then    -- functional tag after the comment
                return string.sub( str,1,m )..head, tag, tail
            end
        else
            return head,tag,tail
        end
    end
        
    return nil  -- no function tag
end

-----
-- str= Loc_ApplyFunctions_str( str )
--
-- Expands Lumikki tags (<@...>) within a given string.
--
local function Loc_ApplyFunctions_str( str )
    --
    ASSUME(str)    

    local head,tag,tail= Loc_NextFuncTag( str )
    local text,params

    if not head then
        return str  -- no function tags
    else
        tag, params= Loc_ParseTag( tag )    -- params into a table

        -- Is there a terminating tag?
        --
        -- Note: Regular tags are allowed within the block but <@..> tags are not.
        --
        local head2,tag2,tail2= skip2( string.find( tail, "(.-)<(/?@.-)>(.*)" ) )

        if tag2 and ( string.lower(tag2) == "/"..string.lower(tag) or
                      tag2=="/@" ) then  -- terminator found
            text= head2
            tail= tail2
        else
            text= nil   -- no terminating tag
            -- tail=tail
        end
    end

    if tail=="" then tail=nil end
        
    -- We know the 'head', we know the function, we know the 'tail'
    --
    local tmp= string.sub(tag,2)
    
    local func= rawget( _G, tmp ) or
                rawget( _G, string.lower(tmp) ) or
                rawget( _G, string.upper(tmp) ) or
                    error( "No function for tag: <"..tag..">" )

    local str, expand= func( params, text )

    ASSUME( str, "Function "..tag.." returned nil! (must return string)" )
    
    -- Recursive use of <@...> tags (if necessary)
    --
    if expand then
        str= Loc_ApplyFunctions_str( str )
    end
        
    if tail then
        tail= Loc_ApplyFunctions_str( tail )
    end
    
    return head..str..(tail or "")
end

-----
-- html_tbl= Loc_ApplyFunctions( html_tbl, fname_str )
--
-- Expands Lumikki tags (<@...>) within the given HTML table (from 'deHTML()').
--
local function Loc_ApplyFunctions( tbl, fname )
    --
    local ret= {}

    current_file= fname

    for i=1,table.getn(tbl) do
        --
        local item= tbl[i]
        
        if string.sub( item[1],1,2 ) == "<@" then
            --
            current_line= item.line

            table.insert( ret, { Loc_ApplyFunctions_str( item[1] ), line=item.line } )
        else
            table.insert( ret, item )
        end
    end
    
    ret.file= tbl.file
    return ret
end

-----
-- tbl= deHTML( html_str [,filename_str] )
--
-- Returns the exact HTML contents (linefeeds etc. all preserved), split by tags,
-- comments etc. to ease handling & modification later on.
--
-- Filename and line numbers are included to help debugging (giving better error 
-- messages to the user).
--
-- Note: The filename provided is merely for error message's sake.
--
function deHTML( html, fname )
    --
    local ret= {}       -- { { str, line=int }, ... , file=str }
    local n1= 1
    local line=1
    local busy= true
    
    while busy do        
        --
        local tmp= string.sub( html,n1,n1+3 )
        local _,n2, catch
        
        if tmp=="<!--" then   -- "<!--...-->"
            --
            _,n2,catch= string.find( html, "(.-%-%->)", n1 )    -- start at n1
            if not catch then
                SYNTAX_ERROR( "comment not closed!", line, fn )
            end
            --
        elseif string.sub(tmp,1,2)=="<@" then    -- "<@func ...>[...</@[func]>]"
            --
            _,n2,catch= string.find( html, "(.->)", n1 )  -- tag itself + params
            if not catch then
                SYNTAX_ERROR( "tag left open!", line, fn )
            end

            local tag= third( string.find( catch, "<(.-)[%s>]" ) )
            ASSUME( tag )

            -- Find next '<@' tag or terminating '</@' tag:
            -- (text may contain HTML formatting, but not another '<@' tag)
            -- 
            local __,n3,mid,endtag= string.find( html, "(.-)(</?@.->)", n2+1 )

            if endtag=="</"..tag..">" or endtag=="</@>" then
                --
--[[
                if string.find( mid, "<!%-%-" ) then
                    -- we could actually allow this, but.. is it needed?
                    -- what if the terminating tag is within that comment, then?
                    --
                    SYNTAX_ERROR( "comments within <@func> block!", line, fname )  -- banned.
                end
]]--                
                catch= catch..mid..endtag   -- all the way to the terminating tag :)
                n2= n3
            end
            --
        elseif string.sub(tmp,1,1)=="<" then    -- "<...>"
            --
            _,n2,catch= string.find( html, "(.->)", n1 )
            if not catch then
                SYNTAX_ERROR( "tag left open!", line, fn )
            end
        else
            _,n2,catch= string.find( html, "(.-)<", n1 )
            
            if not catch then
                catch= string.sub( html, n1 )
                busy= false   -- done!
            else
                n2= n2-1  -- not the '<'
            end
        end

        table.insert( ret, { catch, line=line } )

        -- Count number of newlines within 'catch'
        --
        for _ in string.gfind( catch, '\n' ) do
            line= line+1
        end
        
        if n2 then n1= n2+1 end
    end

    ret.file= fname

    return ret
end

-----
-- str= OutputHTML( html_tbl )
--
function OutputHTML( tbl )
    --
    -- Cannot use 'table.concat' directly, since the strings are in subtables.
    -- Expecting it to be faster than just 'str= str..' a zillion times?
    --
    local tbl2= {}

    for _,v in ipairs(tbl) do
        table.insert( tbl2, v[1] )
    end
    
    return table.concat( tbl2 )
end


--
local m= ( function()

--
-- PUTZ.LUA
--

-----
-- str= Loc_Footer()
--
ASSUME( Loc_ApplyFunctions_str )

local function Loc_Footer()
    --
    local str= "\n<hr><p align=center><font size=\"-3\">This page was created by "..
               "<@LINK popup url=www.sci.fi/~abisoft/lumikki/>Lumikki</@LINK>"..
               "</p>\n"

    return Loc_ApplyFunctions_str( str, true )
end


-----
-- -1/0/+1= Loc_CommentCount( str, start_str, end_str )
--
-- Returns: 0 if comments/pre blocks are balanced (none, or both <!-- and -->)
--          -1 if more stop than start
--          +1 if more start than stop
--
local function Loc_CommentCount( str, pat1, pat2 )
    --
    local _,started= string.gsub( str, pat1, "" )
    local _,ended= string.gsub( str, pat2, "" )
    local n= started - ended

    if n<-1 or n>1 then
        SYNTAX_ERROR( "Recursive comments found!" )
    end
    
    return n
end

-----
-- bool= Loc_NoPtag( str )
--
-- Returns 'true' when the given paragraph seems to be missing the <p> tag.
-- 
local lookup_noptag= { img=1, i=1, b=1, u=1, }  -- tags considered as 'plain text' starter
local lookup_skip=   { font=1, hr=1 }   -- tags that need to be skipped to know

local function Loc_NoPtag( str )
    --
    local tag= third( string.find( str, "^%s-<(.-)[%s>]" ) )   -- get the first tag

    if not tag then
        if string.find( str, "%S" ) then    -- any non-whitespace, huhuu..=?
            return true     -- plain text
        else
            return false    -- just white space *spit*
        end
        --
    elseif string.sub(tag,1,3)=="!--" then  -- starts with a comment (skip it)
        --
        local tail= skip2( string.find( str, "<!%-%-.-%-%->(.+)" ) )
        
        if not tail then    -- no end for comment
            return false
        else
            return Loc_NoPtag( tail )
        end
    else
        local tag_low= string.lower(tag)
        
        if lookup_skip[ tag_low ] then    -- skip that tag..
            --
            local tail= third( string.find( str, "<.->%s*(.+)" ) )    -- skip one tag
            if tail then
                return Loc_NoPtag( tail )
            end
            --
        elseif lookup_noptag[ tag_low ] then
            return true     -- starting by '<img' etc.. (needs <p>)
        end
    end
    
    return false    -- I guess we're allright?
end
  
-----
-- [str]= Loc_ConvertListParagraph( str )
--
-- Converts a set of '- entry' lines to '<ul><li>...</ul>' format, or returns 'nil'.
--
local function Loc_ConvertListParagraph( str )
    --
    local ret= nil

    -- valid <UL> types are: "disc"/"circle"/"square"/...
    --
    local lookup= { ['-']='', ['+']='+', ['o']='circle', ['*']='disc', ['#']='square' }
    
    for s in lines(str) do
        --
        local bullet,tail= skip2( string.find( s, "^%s*([-+o*#])%s+(.+)" ) )
    
        if not bullet then
            return nil  -- some line wasn't a bullet!
        end
        
        if not ret then
            ret= (bullet=='') and "<ul>\n"    -- normal
                               or "<ul type=\""..lookup[bullet].."\">\n"
        end

        ret= ret.."  <li>"..tail.."</li>\n"
    end
    
    if ret then 
        return ret.."</ul>"  -- all lines were list
    else
        return nil
    end
end

-----
-- str= Loc_PutzFilter( str )
--
-- Handles HTML code, filling in lazy-coder's gaps s.a. no '<html>' tag, no '<p>', 
-- plain text list producing (+/-/o) etc.
--
-- This is definately a hack area :) and is secondary to Lumikki in a way (you can
-- use the rest of the system well without this).
--
local within_comment, within_pre
local body_closed, html_closed
local putzed   -- table to collect lines (needs to be accessible to 'Loc_Paragraph()')

local function Loc_Paragraph( s )
    --
    local comment_count= Loc_CommentCount(s,"<!%-%-","%-%->")
    local pre_count= Loc_CommentCount( s, "<[Pp][Rr][Ee][%s>]", "</[Pp][Rr][Ee][%s>]" )
    local postfix
    
    -- Going state-crazy here.. hang on!
    --
    if within_comment then
        if comment_count < 0 then
            within_comment= false   -- no longer within a comment at end of this paragraph
        end
    elseif within_pre then
        if pre_count < 0 then
            within_pre= false       -- no longer within <PRE> block at end of this paragraph
        end
    else
        local tmp= Loc_ConvertListParagraph(s)
        
        if tmp then     -- was a list!  (merge with previous paragraph)
            --
            local last= table.getn(putzed)   -- that's the whitespace
            table.insert( putzed, last, "\n"..tmp )   -- before the whitespace (& </p>)
            
            -- clean away unnecessary white space
            --
            --io.stderr:write( "<<<<"..putzed[last+1]..">>>>\n" )
            putzed[last+1]= string.gsub( putzed[last+1], "%s+$", "" )
            
            s= nil      -- none from us, now
            --
        else
            if Loc_NoPtag( s ) then     -- need to insert <p> ?
                s= "<p>"..s
            end

            if comment_count>0 then
                within_comment= true
            end
            if pre_count>0 then
                within_pre= true
            end
        end
        
        ASSUME( not (within_comment and within_pre) )   -- not prepared for that
        
        -- Check if we need to add a </p> after the paragraph:
        --
        if s and (not within_comment) and (not within_pre) then
            --
            if string.find( s, "^<[Pp][%s>]" ) and  -- must be at the beginning
               (not string.find( s, "</[Pp][%s>]" )) then
               --
               postfix= "\n</p>"  -- most likely, we do..
            end
        end
    end

    -- More effective to store the pieces in a table, then 'table.concat'
    -- (than do concat explicitly here all time)
    --
    if s then
        if string.find( s, "</[Bb][Oo][Dd][Yy][%s>]" ) then
            s= Loc_Footer()..s   -- before the </body> block
            body_closed= true
        end

        if body_closed and string.find( s, "</[Hh][Tt][Mm][Ll][%s>]" ) then
            html_closed= true
        end
    end
    
    return s, postfix
end

-----
-- iterator factory that returns a function.
--
local function Loc_ParagraphIterator( str )
    --
    local pos= 1
    
    return
        -- str1: non-whitespace paragraph text
        -- str2: whitespace (min. two newlines)
        --
        function()
            --        
            if not pos then return nil end  -- done already
            
            -- works even with >2 linefeeds! (also linefeed is %s :)
            --
            local _,_, part1,part2, pos2= string.find( str, "(.-)(\n%s*\n)()", pos )
            
            if not part1 then   -- end of string: rest is last
                --
                _,_, part1,part2, pos2= string.find( str, "(.+)(%s*)", pos )
                pos= nil    -- done
            else
                pos= pos2   -- continue from here..
            end
            
            return part1,part2
        end
end

--
local function Loc_PutzFilter( str )
    --
    local prefix= ""
    local postfix= nil

    putzed= {}
    
    -- Add <html> and <head> if not there:
    --
    if not string.find( str, "<[Hh][Tt][Mm][Ll][%s>]" ) then
        prefix= "<html>"
        postfix= "</html>"
    end
    if not string.find( str, "<[Hh][Ee][Aa][Dd][%s>]" ) then
        prefix= prefix.."<head></head>"
    end
    if not string.find( str, "<[Bb][Oo][Dd][Yy][%s>]" ) then
        prefix= prefix.."<body>"
        postfix= "</body>"..(postfix or "")
    end

    if prefix~="" then
        str= prefix.."\n\n"..str..(postfix and "\n\n"..postfix or "")
    end
    
    -- Process data paragraph-wise
    --
    for part1,part2 in Loc_ParagraphIterator(str) do    -- custom iterator, isn't Lua just great! :)
        --
        --local tmp= string.gsub( part2, '\n', '@' )
        --io.stderr:write( "<<<<<<\n"..part1.."-----"..tmp..">>>>>>\n" )

        local s, postfix= Loc_Paragraph( part1 )
        
        if s then 
            -- Special case: remove newline immediately after <pre> (makes block writes easier).
            --
            -- Note: this is the ONLY place where original HTML meaning is tampered with.
            --
            s= string.gsub( s, "(<[Pp][Rr][Ee]>)%s*\n", "%1" )     -- leave out the '\n'
            
            -- Modify '&' -> '&amp;' if alone.
            --
            s= string.gsub( s, "&%s", "&amp; " )
            s= string.gsub( s, " < ", "&lt; " )
            s= string.gsub( s, " > ", "&gt; " )
            
            -- "*bold*" and "_underlined_"?
            --
            s= string.gsub( s, "(%s)*(%S+)*([%p%s])", "%1<b>%2</b>%3" )
            s= string.gsub( s, "(%s)_(%S+)_([%p%s])", "%1<u>%2</u>%3" )
            
            -- Modify åäö etc.  (so I can type them directly on a Mac ;P
            --
            -- Note: This should really be codepage based; now it's a hack
            --       that works on specific systems (mainly, mine ;) but
            --       not in a general way.
            --
            local repl
            
            if WIN32 then   -- Codepage 850(?)
                repl= { ['Â']="&aring;", ['≈']="&Aring;",
                        ['‰']="&auml;", ['ƒ']="&Auml;",
                        ['ˆ']="&ouml;", ['÷']="&Ouml;",
                        ['¸']="&uuml;", ['‹']="&Uuml;",
                        --
                        ['·']="&aacute;", ['‡']="&agrave;",
                        --...
                      }
            else    -- OS X / Finnish
                repl= { ['å']="&aring;", ['Å']="&Aring;",
                        ['ä']="&auml;", ['Ä']="&Auml;",
                        ['ö']="&ouml;", ['Ö']="&Ouml;",
                        ['ü']="&uuml;", ['Ü']="&Uuml;",
                        --
                        ['ñ']="&ntilde;", ['Ñ']="&Ntilde;",
                        ['á']="&aacute;", ['à']="&agrave;", ['â']="&acirc;",
                        ['é']="&eacute;", ['è']="&egrave;", ['ê']="&ecirc;",
                        ['í']="&iacute;", ['ì']="&igrave;", ['î']="&icirc;",
                        --
                        ['€']="&euro;",   -- "&#8364;"
                        ['©']="&copy;",
                        ['®']="&reg;",
                        --...
                      }
            end
            for k,v in repl do
                s= string.gsub( s, k, v )
            end

            table.insert( putzed, s ) 
        end
        table.insert( putzed, (postfix or "")..part2 )  -- whitespace (and </p>)
    end

    -- Make sure there's a footer & </body> and </html> tags
    --
    if not body_closed then
        table.insert( putzed, Loc_Footer() )
        table.insert( putzed, "\n</body>\n" ..((not html_closed) and "</html>\n" or "") )
    end

    return table.concat( putzed )
end


--
return {
    PutzFilter= ASSUME( Loc_PutzFilter )
    }
end )()

local PutzFilter=   assert( m.PutzFilter )


-----
-- str= CurrentDir()
-- str= CurrentFile()
--
-- Filter functions can use this to query the currently processed file's directory.
--
function CurrentDir()
    --
    if current_file then
        return PathOnly(current_file) or ""
    else
        return ""   -- stdin
    end
end

function CurrentFile()
    return current_file
end


--== Command line usage ==--

local ABOUT=
    "\n** Lumikki website generator (rel.".. mymod._info.RELEASE .."), ".. mymod._info.COPYRIGHT ..
    "\n"
    
local HELP= [[
    lua lumikki.lua input.lhtml [...] >output.htm
    lua lumikki.lua dirname/.
]]

local input= {}
local help= false
local logo= true

for _,v in ipairs(arg) do
    --
    switch( v,
        { "help", function() help=true end },
        { "nologo", function() logo=false end },
        --
        { nil, function() table.insert( input, v ) end }   -- default
        )
end

if logo then
    io.stderr:write( ABOUT.."\n" )      -- copyright text
end
if help then
    io.stderr:write( HELP.."\n" )
end

if not input[1] then
    -- No args = loaded to be used as a library.
    --
    return mymod
else
    ASSUME( not input[2] )    -- just one argument

    local fn= input[1]
    --
    local str= ASSUME( ReadFile(fn), "File not found: "..fn )
    
    local tbl= deHTML( str )      -- all tagwise split, functions expanded

    tbl= Loc_ApplyFunctions( tbl, fn )    -- do all '<@func's>'

    local str= OutputHTML( tbl )    -- to string domain

    str= PutzFilter( str )  -- awful, nasty lazy writer checks & fixes (adding '<p>' etc.)

    io.write(str)
end

--
return mymod
