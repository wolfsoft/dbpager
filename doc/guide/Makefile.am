#
# Makefile.am
# This file is part of dbPager Server
#
# Copyright (c) 2008 Dennis Prochko <wolfsoft@mail.ru>
#
# dbPager Server is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation version 3.
#
# dbPager Server is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with dbPager Server; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, 
# Boston, MA  02110-1301  USA
#

guide_dir=@docdir@/guide

all-local:
	find -iname '*.xml' -exec echo \"$(xsltproc) --stringparam \
	file-extension 'html' -o '{}' '{}'\" ';' | $(sed) s/.xml/.html/|xargs -n1 sh -c

maintainer-clean-local:
	find -iname '*.html' -exec rm -f '{}' ';'

clean-local:
	find -iname '*.html' -exec rm -f '{}' ';'

dist-hook: clean-local
	$(mkinstalldirs) $(distdir)
	tar --exclude='.svn' -cf - . | tar -C $(distdir) -xvf -

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(guide_dir)
	$(mkinstalldirs) $(DESTDIR)$(guide_dir)/images
	$(mkinstalldirs) $(DESTDIR)$(guide_dir)/style
	current_dir=`pwd`
	cd images/ && tar --exclude='.svn' -cf - . | tar -C $(DESTDIR)$(guide_dir)/images -xvf -
	cd $(current_dir)
	cd style/ && tar --exclude='.svn' -cf - . | tar -C $(DESTDIR)$(guide_dir)/style -xvf -
	cd $(current_dir)
	find -iname '*.html' -exec cp -a --parents '{}' $(DESTDIR)$(guide_dir) ';'

uninstall-local:
	if test -d $(DESTDIR)$(guide_dir); then \
		rm -rf $(DESTDIR)$(guide_dir)/*; \
		rmdir $(DESTDIR)$(guide_dir) || true; \
	fi

