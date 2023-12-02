#!/usr/bin/perl

($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst)=localtime(time);

$remoteaddr = $ENV{'REMOTE_ADDR'};

# Read and parse input from the web form

read(STDIN, $buffer, $ENV{'CONTENT_LENGTH'});

@pairs = split(/&/, $buffer);
foreach $pair (@pairs) {
    ($name, $value) = split(/=/, $pair);
    $value =~ tr/+/ /;
    $value =~ s/%([a-fA-F0-9][a-fA-F0-9])/pack("C", hex($1))/eg;
    $value =~ s/<!--(.|\n)*-->//g;
    $input{$name} = $value;
}
local(*MAIL);
if(open(MAIL, '|/usr/sbin/sendmail -t -fCbVvoXsd tonigy@defgroup.com')) {

    print MAIL "To: \"DEF Group\" <tonigy\@defgroup.com>\n";
    print MAIL "From: <tonigy\@defgroup.com>\n";
    print MAIL "Subject: On-line feedback\n\n";

    print MAIL "   time: $sec:$min:$hour $mday/$mon\n";
    print MAIL "     ip: $remoteaddr\n";
    print MAIL "   from: $input{'name'} <$input{'email'}>\n";
    print MAIL " e-mail: $input{'email'}\n\n";

    print MAIL "$input{'message'}\n";
    close(MAIL);

    # Send a message back to the user
  
    print "Status: 302\n";
    print "Location: http://www.defgroup.com/tonigy/sent.html\n\n";
#    print "Content-Type: text/html\n\n";
#    print "OK!\n";

} else {
    print "Content-Type: text/html\n\n";
    print "Internal error!\n";
}
