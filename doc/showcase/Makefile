# This file is part of the Home2L project.
#
# (C) 2015-2020 Gundolf Kiefer
#
# Home2L is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Home2L is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Home2L. If not, see <https://www.gnu.org/licenses/>.


# This file is NOT part of the Home2L build system and contains just some
# internal development targets.


TMPDIR=/tmp/home2l


.PHONY: vmpack
vmpack:
	rm -fr $(TMPDIR)/vmpack
	mkdir -p $(TMPDIR)/vmpack
	cp -a home2l-demo-vm $(TMPDIR)/vmpack
	gunzip $(TMPDIR)/vmpack/home2l-demo-vm/system.vdi.gz
	tar czf home2l-demo-vm.tar.gz -C $(TMPDIR)/vmpack home2l-demo-vm
	rm -fr $(TMPDIR)/vmpack


.PHONY: vminstall
vminstall:
	rm -fr `realpath vm-ram`
	mkdir -p `realpath vm-ram`
	tar xzf home2l-demo-vm.tar.gz -C vm-ram
	ln -s $$PWD/debian*/debian-*.iso vm-ram/install.iso


.PHONY: vmrun
vmrun:
	virtualbox --startvm home2l-demo-vm &
