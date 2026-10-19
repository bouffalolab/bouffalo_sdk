#!/usr/bin/perl

open(OUTPUT, "> fsdata.c");

chdir("web_page");
open(FILES, "find . -type f |");

while($file = <FILES>) {

    # Do not include files in CVS directories nor backup files.
    if($file =~ /(CVS|~)/) {
    	next;
    }
    
    chop($file);
    
    open(HEADER, "> /tmp/header") || die $!;
    print(HEADER "\r\n");
    close(HEADER);

    unless($file =~ /\.plain$/ || $file =~ /cgi/) {
	system("cat /tmp/header $file > /tmp/file");
    } else {
	system("cp $file /tmp/file");
    }
    
    open(FILE, "/tmp/file");
    unlink("/tmp/file");
    unlink("/tmp/header");

    $file =~ s/\.//;
    $fvar = $file;
    $fvar =~ s-/-_-g;
    $fvar =~ s-\.-_-g;
    print(OUTPUT "static const unsigned char data".$fvar."[] = {\n");
    print(OUTPUT "\t/* $file */\n\t");
    for($j = 0; $j < length($file); $j++) {
	printf(OUTPUT "%#02x, ", unpack("C", substr($file, $j, 1)));
    }
    printf(OUTPUT "0,\n");
    
    
    $i = 0;
    while(read(FILE, $data, 1)) {
        if($i == 0) {
            print(OUTPUT "\t");
        }
        printf(OUTPUT "%#02x, ", unpack("C", $data));
        $i++;
        if($i == 10) {
            print(OUTPUT "\n");
            $i = 0;
        }
    }
    print(OUTPUT "};\n\n");
    close(FILE);
    push(@fvars, $fvar);
    push(@files, $file);
}

for($i = 0; $i < @fvars; $i++) {
    $file = $files[$i];
    $fvar = $fvars[$i];

    if($i == 0) {
        $prevfile = "NULL";
    } else {
        $prevfile = "file" . $fvars[$i - 1];
    }
    print(OUTPUT "const struct fsdata_file file".$fvar."[] = {{$prevfile, data$fvar, ");
    print(OUTPUT "data$fvar + ". (length($file) + 1) .", ");
    print(OUTPUT "sizeof(data$fvar) - ". (length($file) + 1) ."}};\n\n");
}

print(OUTPUT "#define FS_ROOT file$fvars[$i - 1]\n\n");
print(OUTPUT "#define FS_NUMFILES $i\n");
