# This file is part of the Home2L project.
#
# (C) 2015-2018 Gundolf Kiefer
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


MYDIR := $(dir $(lastword $(MAKEFILE_LIST)))

# Use the Debian packages, not these sources...
#~ LDFLAGS += -lSDL2 -lSDL2_ttf


ifneq (,$(wildcard $(MYDIR)usr/$(ARCH)/include))
  # Use these sources...
  CFLAGS += -I$(MYDIR)usr/$(ARCH)/include
  LDFLAGS += -L$(MYDIR)usr/$(ARCH)/lib -lmpdclient
else
  # Use (fallback) Debian packages, not these sources...
  LDFLAGS += -lmpdclient
endif
