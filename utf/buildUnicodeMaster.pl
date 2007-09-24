#!/usr/bin/perl
use integer;

# Korean Hangul Constants.
#
$SBase = 0xAC00;
$LBase = 0x1100;
$VBase = 0x1161;
$TBase = 0x11A7;
$SCount = 11172;
$LCount = 19;
$VCount = 21;
$TCount = 28;
$NCount = $VCount * $TCount;

my $line;

my @codepoints = ();
$#codepoints = 1114109;

my $unicodedata = $ARGV[0];
my $secondary = $ARGV[1];

open(UNICODE,"< $unicodedata") || die("Eep, no UnicodeData.");

while ($line = <UNICODE>)
{
    if ($line =~ /^([0-9A-F]+);<(.*), First>;([^;]*;[^;];*[^;]*);([^;]*);(.*)/)
    {
        $range_point1 = $1;
        $range_desc1 = $2;
        $range_fields1A = $3;
        $range_fields1B = $4;
        $range_fields1C = $5;
    }
    elsif ($line =~ /^([0-9A-F]+);<(.*), Last>;([^;]*;[^;];*[^;]*);([^;]*);(.*)/)
    {
        my $range_point2 = $1;
        my $range_desc2 = $2;
        my $range_fields2A = $3;
        my $range_fields2B = $4;
        my $range_fields2C = $5;

        if (  $range_desc1 eq $range_desc2
           && $range_fields1A eq $range_fields2A
           && $range_fields1B eq $range_fields2B
           && $range_fields1C eq $range_fields2C)
        {
            $codeval1 = hex $range_point1;
            if ($codeval1 == $SBase)
            {
                # Korean Hangul.
                #
                my $codeval2 = hex $range_point2;
                for ($loop = $codeval1; $loop <= $codeval2; $loop++)
                {
                    my $SIndex = $loop - $SBase;
                    my $L = $LBase + $SIndex / $NCount;
                    my $V = $VBase + ($SIndex % $NCount) / $TCount;
                    my $T = $TBase + $SIndex % $TCount;

                    if ($T == $TBase)
                    {
                        $decomposition = sprintf("%04X %04X", $L, $V);
                    }
                    else
                    {
                        $decomposition = sprintf("%04X %04X %04X", $L, $V, $T);
                    }
                    $codepoints[$loop] = "$range_desc1;$range_fields1A;$decomposition;$range_fields1C";
                }
            }
            else
            {
                # non-Hangul.
                #
                my $codeval2 = hex $range_point2;
                for ($loop = $codeval1; $loop <= $codeval2; $loop++)
                {
                    $codepoints[$loop] = "$range_desc1;$range_fields1A;$range_fields1B;$range_fields1C";
                }
            }
        }
        else
        {
            print "***ERROR: $range_point1 does not agree with $range_point2\n";
        }
    }
    elsif ($line =~ /^([0-9A-F]+);(.*)/)
    {
        $unicodeval = hex $1;
        $codepoints[$unicodeval] = $2;
    }
}

close (UNICODE);

open (CJKRANGE, "< $secondary") || die ("Eep, no secondary data!");

while ($line = <CJKRANGE>)
{
    if ($line =~ /^\s*([0-9A-F\-]+);(.*)/)
    {
        my $codepage = $1;
        my $codename = $2;
        if ($codepage =~ /^([0-9A-F]+)\-([0-9A-F]+)$/)
        {
             my $codeval1 = hex $1;
             my $codeval2 = hex $2;
             for ($loop = $codeval1; $loop <= $codeval2; $loop++)
             {
                 my $temphex = sprintf("%04X", $loop);
                 $codepoints[$loop] = "$codename CHARACTER $temphex;So;0;ON;;;;N;;;;;";
             }
        }
        else
        {
            my $codeval = hex $codepage;
            $codepoints[$codeval] = "$codename CHARACTER $codepage;So;0;ON;;;;N;;;;;";
        }
    }
}

for ($loop = 0; $loop < 1114110; $loop++)
{
    if (defined $codepoints[$loop])
    {
        my $temphex = sprintf("%04X", $loop);
        print "$temphex;$codepoints[$loop]\n";
    }
}