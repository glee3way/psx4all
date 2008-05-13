/************************************************************************

Copyright mooby 2002

CDRMooby2 SubchannelData.cpp
http://mooby.psxfanatics.com

  This file is protected by the GNU GPL which should be included with
  the source code distribution.

************************************************************************/

#include "../common.h"

#include "SubchannelData.hpp"

using namespace std;

#include <fstream>

// tries to open the subchannel files to determine which
// one to use
SubchannelData* SubchannelDataFactory(const std::string& fileroot)
{
   SubchannelData* scd = NULL;
   ifstream testFile;
   testFile.open( (fileroot + std::string(".sub")).c_str());
   if (testFile)
   {
      scd = new SUBSubchannelData();
      scd->openFile(fileroot + std::string(".sub"));
      return scd;
   }
   
   testFile.open( (fileroot + std::string(".sbi")).c_str());
   if (testFile)
   {
      scd = new SBISubchannelData();
      scd->openFile(fileroot + std::string(".sbi"));
      return scd;
   }
   testFile.open( (fileroot + std::string(".m3s")).c_str());
   if (testFile)
   {
      scd = new M3SSubchannelData();
      scd->openFile(fileroot + std::string(".m3s"));
      return scd;
   }
   else
   {
      scd = new NoSubchannelData();
      return scd;
   }
}

// SUB files read from the file whenever data is needed
void SUBSubchannelData::openFile(const string& file)
   throw(Exception)
{
   subFile.open(file.c_str(), ios::binary);
   subFile.exceptions(ios::failbit);
}

void SUBSubchannelData::seek(const CDTime& cdt)
   throw(Exception)
{
	// seek in the file for the data requested and set the subframe
	// data
   try
   {
      CDTime seekTime(cdt - CDTime(0,2,0));
      subFile.seekg(seekTime.getAbsoluteFrame() * SubchannelFrameSize);
      subFile.read((char*)sf.subData, SubchannelFrameSize);
   }
   catch(std::exception& e)
   {
      Exception exc("Exception reading SUB file");
      exc.addText(string(e.what()));
      THROW(exc);
   }
}

// opens the SBI file and caches all the subframes in a map
void SBISubchannelData::openFile(const std::string& file) 
      throw(Exception)
{
   ifstream subFile(file.c_str(), ios::binary);
   subFile.exceptions(ios::failbit|ios::badbit|ios::eofbit);

   try
   {
      unsigned char buffer[4];
      subFile.read((char*)&buffer, 4);
      if (string((char*)&buffer) != string("SBI"))
      {
         Exception e(file + string(" isn't an SBI file"));
         THROW(e);
      }
      while (subFile)
      {
         subFile.read((char*)&buffer, 4);
         CDTime now((unsigned char*)&buffer, msfbcd);
         SubchannelFrame subf(now);

         switch(buffer[3])
         {
         case 1:
            subFile.read((char*)&subf.subData[12], 10);
            break;
         case 2:
            subFile.read((char*)&subf.subData[15], 3);
            break;
         case 3:
            subFile.read((char*)&subf.subData[19], 3);
            break;
         default:
            Exception e("Unknown buffer type in SBI file");
            THROW(e);
            break;
         }
         subMap[now] = subf;
      }
   }
   catch(Exception&)
   {
      throw;
   }
   catch (std::exception& e)
   {
      if (!subFile.eof())
      {
         Exception exc("Error reading SBI file");
         exc.addText(string(e.what()));
         THROW(exc);
      }
   }
}

// if the data is in the map, return it.  otherwise, make up data
void SBISubchannelData::seek(const CDTime& cdt)
   throw(Exception)
{
   map<CDTime, SubchannelFrame>::iterator itr = subMap.find(cdt);
   if (itr == subMap.end())
   {
      sf.setTime(cdt);
   }
   else
   {
      sf = (*itr).second;
   }
}

// opens and caches the M3S data
void M3SSubchannelData::openFile(const std::string& file) 
   throw(Exception)
{
   ifstream subFile(file.c_str(), ios::binary);
   subFile.exceptions(ios::failbit|ios::badbit|ios::eofbit);

   try
   {
      CDTime t(3,0,0);
      char buffer[16];
      while (subFile)
      {
         subFile.read((char*)&buffer, 16);
         SubchannelFrame subf(t);
         memcpy(&subf.subData[12], buffer, 10);
         t += CDTime(0,0,1);         
         if (t == CDTime(4,0,0))
            break;
      }
   }
   catch(std::exception& e)
   {
      Exception exc("Error reading M3S file");
      exc.addText(string(e.what()));
      THROW(exc);
   }
}

// if no data is found, create data. otherwise, return the data found.
void M3SSubchannelData::seek(const CDTime& cdt)
   throw(Exception)
{
   map<CDTime, SubchannelFrame>::iterator itr = subMap.find(cdt);
   if (itr == subMap.end())
   {
      sf.setTime(cdt);
   }
   else
   {
      sf = (*itr).second;
   }
}
