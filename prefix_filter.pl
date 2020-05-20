#! /usr/bin/env perl
#
# Short description for prefix_filter.pl
#
# Author Shlomi Fish <shlomif@cpan.org>
# Version 0.0.1
# Copyright (C) 2020 Shlomi Fish <shlomif@cpan.org>
#
use strict;
use warnings;
use 5.014;
use autodie;

use Path::Tiny qw/ path tempdir tempfile cwd /;
my $prefix = q#fcs_states_#;
my $CDIR   = "source";
foreach
    my $src ( "$CDIR/sentinel.c", "$CDIR/table.c", "include/OB_table/table.h" )
{
    my $src_p  = path("./$src");
    my $dest_p = path("./states_ob/$src");

    $dest_p->touchpath->spew_utf8(
        map {
            my $s = (
                /\A\#/
                ? (
                      /\A#include <OB_table\/table\.h>/
                    ? "#include \"states_ob/include/OB_table/table.h\"\n"
                    : $_
                    )
                : (
s/\A((SHRET|void *\*( *\*)?|struct OB_item *\*|size_t) *(OB\w+)\()/static $1/gr
                        =~ s/\b(OB_)/$prefix$1/gr =~
                        s/t\-\>hash\(el\)/fcs_states_myhash(el)/gr =~
                        s/t\-\>comp\(/fcs_states_ob_mycompare(/gr )
            );
            $s =~ s/\b(VV_)/$1_$prefix$1/g;
            if ( $src =~ /table\.c/ )
            {
                my $NOT_NEEDED = qq%#include "state.h"\n%;
                $s =
qq%#define fcs_states_ob_mycompare(a, b) (!fc_solve_state_compare((a), (b)))\n\n%
                    . $s;
            }
            $s
        } $src_p->lines_utf8
    );
}
