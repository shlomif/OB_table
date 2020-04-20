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
foreach my $src ( "OB_sentinel.c", "OB_table.c", "OB_table.h" )
{
    my $src_p  = path("./$src");
    my $dest_p = path("./states_ob/$src");

    $dest_p->touchpath->spew_utf8(
        map {
            /\A\#/
                ? $_
                : s/\A((SHRET|void *\*( *\*)?|struct OB_item *\*|size_t) *(OB\w+)\()/static $1/gr
                =~ s/\b(OB_)/$prefix$1/gr =~
                s/t\-\>hash\(el\)/fcs_states_myhash(el)/gr;
        } $src_p->lines_utf8
    );
}
