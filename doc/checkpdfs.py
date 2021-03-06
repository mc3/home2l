#!/usr/bin/python3

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


import os
import PyPDF2
import requests


def CheckFile (fileName, locPath):
  print ("Checking '{}'...".format (fileName))
  error = False

  f = open (fileName, "rb")
  pdf = PyPDF2.PdfFileReader (f)
  pages = pdf.getNumPages ()

  # Extract URIs...
  uriSet = {}
  for pageNo in range (pages):
    oPage = pdf.getPage (pageNo).getObject ()
    if "/Annots" in oPage:
      for anno in oPage ["/Annots"]:
        ank = anno.getObject () ["/A"]
        uri = None
        if "/URI" in ank:
          uri = ank["/URI"]
        elif "/F" in ank:
          uri = ank["/F"]
        elif ank["/S"] != "/GoTo":
          print ("ERROR: Strange annotation on page {}: {}".format (pageNo + 1, str (ank)))
          error = True
        if uri:
          # ~ print ("{:>3}: {}".format (pageNo + 1, uri))
          if not uri in uriSet: uriSet[uri] = set()
          uriSet[uri].add (pageNo + 1)

  # Check the URIs...
  extUrls = None
  for uri, pageSet in uriSet.items ():
    # ~ print ("Checking '{}' on page(s) {} ...".format (uri, pageSet))
    if "://" in uri:
      # Check external web reference ...
      if extUrls: extUrls = "{} '{}'".format (extUrls, uri)
      else:       extUrls = "'{}'".format (uri)
      ok = False
      try:
        req = requests.get (uri)
        if req.status_code == 200: ok = True
      except: pass
      if not ok: print ("ERROR: Non-existing remote URL '{}' {}.".format (uri, sorted (pageSet)))
    else:
      # Check local file ...
      uri = uri.rstrip(".")
        # Due to a bug in the LaTeX 'hyperref' package, it appears to be impossible to refer to files
        # or directories without a period in their name. As a workaround, affected link targets end
        # with a period (''.'') (see also: comment in 'home2l-book.tex').
        # These extra trailing periods are removed by this.
      ok = os.path.exists (locPath + "/" + uri)
      if not ok: ok = os.path.exists (uri)
      if not ok: print ("ERROR: Non-existing local file '{}' {}.".format (uri, sorted (pageSet)))
    if not ok: error = True

  # Done ...
  print ("  To open all external URLs in a web browser, run:\n  $ firefox " + extUrls)
  return not error


if not CheckFile ("../README.pdf", ".."): exit (1)
if not CheckFile ("home2l-book.pdf", ".."): exit (1)
