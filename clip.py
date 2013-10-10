#!/usr/bin/python


import os, sys
import glob

fisiere=glob.glob("*.shp")

for fisier in fisiere:
  for xmin in range(20,30):
    for ymin in range(40,49):
      xarg = "-clipdst "+str(xmin)+" "+ str(ymin) +" " +str(xmin+1)+" "+ str(ymin+1) +" " + "cropped/N"+str(ymin)+"E0"+str(xmin)+"_"+fisier+" "+fisier

      os.system("ogr2ogr "+ xarg)