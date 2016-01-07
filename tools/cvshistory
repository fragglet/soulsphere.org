#!/usr/bin/env perl
#
# Copyright(C) Simon Howard
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
# SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
# IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
#
#
# A perl script to generate a history of CVS commits
#

use strict;
use Date::Manip;

my $quiet = 0;
my %changes = {};

sub get_change {
	my ($date, $message, $author) = @_;

	my $key = int($date / 600) . ": $author : $message";

	if (!$changes{$key}) {
		$changes{$key} = {
			files => [],
			date => $date,
			message => $message,
			author => $author,
		};
	}

	return $changes{$key};
}

sub read_file {
	my $filename;

	# read headers

	while (<CVSPIPE>) {
		chomp;
		last if (/^\-+$/);

		if (/^Working file\:/) {
			($filename) = /^Working file\: (.*)$/;
		}
	}

	if (!$quiet) {
		print STDERR "$filename\n";
	}

	# read revisions

	mainloop: while(1) {
		$_ = <CVSPIPE>; chomp;
		my ($revision) = /^revision (.*)/;

		$_ = <CVSPIPE>; chomp;
		my ($date) = /date\: ([^\;]*)\;/;
		$date = &UnixDate($date,"%s");

		my ($author) = /author: ([^\;]*)\;/;

		my $message = "";

		while (<CVSPIPE>) {
			chomp;
			last mainloop if /^\=+$/;
			last if /^\-+$/;
			next if /^branches\:/;

			$message .= "$_\n";
		}

		$message =~ s/^\n*//;
		$message =~ s/\n*$//;

		my $change = get_change($date, $message, $author);
		my $files = $change->{files};

		push @$files, "$filename: $revision";
	}
}

sub get_repository_name {
	open (REPFILE, "CVS/Repository") or die "cant open CVS/Repository";

	my $repname = <REPFILE>;
	chomp $repname;

	close (REPFILE);

	return $repname;
}

sub output_report {
	my ($file) = @_;
	my $title = "CVS History for '" . get_repository_name() . "'";

	open(REPORT, ">$file") or die "cant open $file";

	print REPORT "<html>\n";
	print REPORT "<head><title>$title</title></head>\n";
	print REPORT "<body>\n";

	my @changekeys = sort {$changes{$a}->{date} <=> $changes{$b}->{date}}
					keys %changes;

	print REPORT "<h1>$title</h1>\n";

	foreach (@changekeys) {
		next if !$changes{$_}->{date};

		my $change = $changes{$_};
		print REPORT "<p>\n";
		print REPORT "<table border='1'>\n";

		print REPORT "<tr><td><b>Date</b></td>\n";
		print REPORT "<td>" . localtime($change->{date})
				    . "</td></tr>\n";

		print REPORT "<tr><td><b>User</b></td>\n";
		print REPORT "<td> $change->{author} </td> </tr>\n";

		print REPORT "<tr><td><b>Files</b></td>";

		my $files = $change->{files};

		print REPORT "<td>";
		foreach (@$files) {
			print REPORT "$_, ";
		}

		print REPORT "</td></tr>\n";

		my $message = $change->{message};
		$message =~ s/\n/\<br\>\n/;

		print REPORT "<tr><td><b>Message</b></td>\n";
		print REPORT "<td><i>$message</i></td></tr>\n";

		print REPORT "</table>\n";
	}

	print REPORT "</table>\n</body></html>\n\n";

	close(REPORT);
}

# output report to console

sub console_report {
	my @changekeys = sort {$changes{$a}->{date} <=> $changes{$b}->{date}}
					keys %changes;

	foreach (@changekeys) {
		next if !$changes{$_}->{date};

		my $change = $changes{$_};

		print "Time: " . localtime($change->{date}) . "\n";

		my $files = $change->{files};

		print "Files: ";
		my $curwid = 0;

		foreach (@$files) {
			if ($curwid + length("$_, ") > 70) {
				$curwid = 0;
				print "\n       ";
			}
			print "$_, ";
			$curwid += length("$_, ");
		}
		print "\n";
		print "Author: $change->{author}\n";
		print $change->{message} . "\n";
		print '-' x 78 ."\n";
	}
}

sub usage {
	print "Usage: $0 [-q] [-h] [-o file]\n";
	print "\n";
	print " -h: Print help information\n";
	print " -q: Quiet mode\n";
	print " -o: Output summary in HTML format to a file\n";
	print "     If -o is not specified, an ascii report is output to the console.\n";
	exit;
}

my $mode = 'console';
my $outfile = "";

# parse command line

for (my $i=0; $i<@ARGV; ++$i) {
	if ($ARGV[$i] eq '-o' && $i+1 < @ARGV) {
		++$i;
		$outfile = $ARGV[$i];
		$mode = 'html';
	}
	if ($ARGV[$i] eq '-h') {
		usage;
	}
	if ($ARGV[$i] eq '-q') {
		$quiet = 1;
	}
}

if (!-e 'CVS') {
	print "Error: there is no checked out CVS here. Try 'cvs checkout' first.\n";
	exit -1;
}

open (CVSPIPE, "cvs -q log |");

if (!$quiet) {
	print STDERR "Reading files..\n";
}

while (<CVSPIPE>) {
	read_file;
}

close(CVSPIPE);

if ($mode eq 'html') {
	output_report $outfile;
} else {
	console_report;
}

