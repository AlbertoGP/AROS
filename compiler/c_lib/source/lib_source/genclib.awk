BEGIN {
    stderr="/dev/stderr";

    file = "libdefs.h";

    while ((getline < file) > 0)
    {
	if ($2 == "BASENAME")
	{
	    lib = $3;
	    basename = $3;
	}
	else if ($2 == "LIBBASE")
	{
	    libbase = $3;
	}
	else if ($2 == "LIBBASETYPEPTR")
	{
	    libbtp = $3;
	    for (t=4; t<=NF; t++)
		libbtp=libbtp" "$t;
	}
    }

    verbose_pattern = libbase"[ \\t]*,[ \\t]*[0-9]+[ \\t]*,[ \\t]*"basename;

    close (file);

    BASENAME=toupper(basename);

    print "#ifndef CLIB_"BASENAME"_PROTOS_H"
    print "#define CLIB_"BASENAME"_PROTOS_H"
    print ""
    print "/*"
    print "    (C) 1995-98 AROS - The Amiga Replacement OS"
    print "    $""Id$"
    print ""
    print "    Desc: Prototypes for "basename".library"
    print "    Lang: english"
    print "*/"
    print ""
    print "#ifndef AROS_LIBCALL_H"
    print "#   include <aros/libcall.h>"
    print "#endif"
    print ""

    file = "headers.tmpl"
    doprint = 0;

    while ((getline < file) > 0)
    {
	if ($1=="##begin" && $2 == "clib")
	    doprint = 1;
	else if ($1=="##end" && $2 == "clib")
	    doprint = 0;
	else if (doprint)
	    print;
    }

    print "/* Prototypes */"
}
/AROS_LH(A|(QUAD)?[0-9])/ {
    line=$0;
    isarg=match(line,/AROS_LHA/);
    gsub(/AROS_LH/,"AROS_LP",line);
    gsub(/^[ \t]+/,"",line);
    if (!isarg)
    {
	call=line;
	narg=0;
    }
    else
    {
	arg[narg++]=line;
    }
}
/LIBBASE[ \t]*,[ \t]*[0-9]+/ || $0 ~ verbose_pattern {
    line=$0;
    gsub(/LIBBASETYPEPTR/,libbtp,line);
    gsub(/LIBBASE/,libbase,line);
    gsub(/BASENAME/,basename,line);
    gsub(/[ \t]*[)][ \t]*$/,"",line);
    gsub(/^[ \t]+/,"",line);
    na=split(line,a,",");
    lvo=int(a[3]);

    if (lvo > 4)
    {
	print call
	for (t=0; t<narg; t++)
	    print "\t"arg[t]
	print "\t"line")";
	print ""
    }
    narg=0;
}
END {
    print "#endif /* CLIB_"BASENAME"_PROTOS_H */"
}
