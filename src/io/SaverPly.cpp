//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin / 3D Shape Tech LLC
//  Time-stamp: <2025-08-07 20:15:36 taubin>
//------------------------------------------------------------------------
//
// SaverPly.cpp
//
// Software developed for the course
// Digital Geometry Processing
// Copyright (c) 2025, Gabriel Taubin
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//     * Redistributions of source code must retain the above
//       copyright notice, this list of conditions and the following
//       disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials
//       provided with the distribution.
//     * Neither the name of the Brown University nor the names of its
//       contributors may be used to endorse or promote products
//       derived from this software without specific prior written
//       permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL GABRIEL
// TAUBIN BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
// USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.

#include "SaverPly.hpp"
#include <wrl/Shape.hpp>
#include <wrl/Appearance.hpp>
#include <wrl/Material.hpp>
#include <wrl/IndexedFaceSet.hpp>
#include <wrl/IndexedFaceSetPly.hpp>
#include <io/StrException.hpp>
#include <util/Endian.hpp>
#include <util/CastMacros.hpp>

#include <iostream>
using namespace std;

const char*   SaverPly::_ext = "ply";
Ply::DataType SaverPly::_defaultDataType = Ply::DataType::BINARY_LITTLE_ENDIAN;
bool SaverPly::_skipAlpha = true;
ostream* SaverPly::_ostrm = nullptr;
string SaverPly::_indent = "";

//////////////////////////////////////////////////////////////////////
SaverPly::SaverPly():_dataType(_defaultDataType) {
}

//////////////////////////////////////////////////////////////////////
// virtual
const char* SaverPly::ext() const {
  return _ext;
}

//////////////////////////////////////////////////////////////////////
void SaverPly::setDataType(const Ply::DataType dataType) {
  _dataType = dataType;
}

//////////////////////////////////////////////////////////////////////
// static
void SaverPly::setDefaultDataType(const Ply::DataType dataType) {
  _defaultDataType = dataType;
}

//////////////////////////////////////////////////////////////////////
// static
void SaverPly::setSkipAlpha(const bool value) {
  _skipAlpha = value;
}

//////////////////////////////////////////////////////////////////////
// static
void SaverPly::setOstream(ostream* ostrm) {
  _ostrm = ostrm;
}

//////////////////////////////////////////////////////////////////////
// static
void SaverPly::setIndent(const string s) {
  _indent = s;
}

//////////////////////////////////////////////////////////////////////
// static
Ply::DataType SaverPly::systemEndian() {
  return
    (Endian::isLittleEndianSystem())?
    Ply::DataType::BINARY_LITTLE_ENDIAN:
    Ply::DataType::BINARY_BIG_ENDIAN;
}

//////////////////////////////////////////////////////////////////////
// static
bool SaverPly::sameAsSystemEndian(Ply::DataType fileEndian) {
  return (fileEndian==systemEndian());
}

//////////////////////////////////////////////////////////////////////
// static
bool SaverPly::writeBinaryValue
(FILE * fp, const Ply::Element::Property::Type listType,
 const bool swapBytes, int nList) {
  bool success = false;
  if(fp!=nullptr) {
    Endian::SingleValueBuffer svb;
    switch(listType) {
    case Ply::Element::Property::Type::CHAR:
    case Ply::Element::Property::Type::INT8:
      svb.c[0] = static_cast<char>(nList);
      success = (fwrite(&(svb.c[0]),1,1,fp)==1);
      break;
    case Ply::Element::Property::Type::UCHAR:
    case Ply::Element::Property::Type::UINT8:
      svb.uc[0] = static_cast<uchar>(nList);
      success = (fwrite(&(svb.uc[0]),1,1,fp)==1);
      break;
    case Ply::Element::Property::Type::SHORT:
    case Ply::Element::Property::Type::INT16:
      svb.s[0] = static_cast<short>(nList);
      if(swapBytes) Endian::swapShort(svb);
      success = (fwrite(&(svb.s[0]),1,2,fp)==2);
      break;
    case Ply::Element::Property::Type::USHORT:
    case Ply::Element::Property::Type::UINT16:
      svb.us[0] = static_cast<ushort>(nList);
      if(swapBytes) Endian::swapUShort(svb);
      success = (fwrite(&(svb.us[0]),1,2,fp)==2);
      break;
    case Ply::Element::Property::Type::INT:
    case Ply::Element::Property::Type::INT32:
      svb.i[0] = static_cast<int>(nList);
      if(swapBytes) Endian::swapInt(svb);
      success = (fwrite(&(svb.i[0]),1,4,fp)==4);
      break;
    case Ply::Element::Property::Type::UINT:
    case Ply::Element::Property::Type::UINT32:
      svb.ui[0] = static_cast<uint>(nList);
      if(swapBytes) Endian::swapUInt(svb);
      success = (fwrite(&(svb.ui[0]),1,4,fp)==4);
      break;
    default:
      break;
    }
  }
  return success;
}

//////////////////////////////////////////////////////////////////////
// static
bool SaverPly::writeBinaryValue
(FILE * fp, const Ply::Element::Property::Type propertyType,
 const bool swapBytes, void* value, int index) {
  bool success = false;
  if(fp!=nullptr) {
    Endian::SingleValueBuffer svb;
    switch(propertyType) {
    case Ply::Element::Property::Type::CHAR:
    case Ply::Element::Property::Type::INT8:
      {
        svb.c[0] = (*static_cast<vector<char>*>(value))[UL(index)];
        success = (fwrite(&(svb.c[0]),1,1,fp)==2);
      }
      break;
    case Ply::Element::Property::Type::UCHAR:
    case Ply::Element::Property::Type::UINT8:
      {
        svb.uc[0] = (*static_cast<vector<uchar>*>(value))[UL(index)];
        success = (fwrite(&(svb.uc[0]),1,1,fp)==2);
      }
      break;
    case Ply::Element::Property::Type::SHORT:
    case Ply::Element::Property::Type::INT16:
      {
        svb.s[0] = (*static_cast<vector<short>*>(value))[UL(index)];
        if(swapBytes) Endian::swapShort(svb);
        success = (fwrite(&(svb.s[0]),1,2,fp)==2);
      }
      break;
    case Ply::Element::Property::Type::USHORT:
    case Ply::Element::Property::Type::UINT16:
      {
        svb.us[0] = (*static_cast<vector<ushort>*>(value))[UL(index)];
        if(swapBytes) Endian::swapUShort(svb);
        success = (fwrite(&(svb.us[0]),1,2,fp)==2);
      }
      break;
    case Ply::Element::Property::Type::INT:
    case Ply::Element::Property::Type::INT32:
      {
        svb.i[0] = (*static_cast<vector<int>*>(value))[UL(index)];
        if(swapBytes) Endian::swapInt(svb);
        success = (fwrite(&(svb.i[0]),1,4,fp)==4);
      }
      break;
    case Ply::Element::Property::Type::UINT:
    case Ply::Element::Property::Type::UINT32:
      {
        svb.ui[0] = (*static_cast<vector<uint>*>(value))[UL(index)];
        if(swapBytes) Endian::swapUInt(svb);
        success = (fwrite(&(svb.ui[0]),1,4,fp)==4);
      }
      break;
    case Ply::Element::Property::Type::FLOAT:
    case Ply::Element::Property::Type::FLOAT32:
    case Ply::Element::Property::Type::FLOAT32_2:
    case Ply::Element::Property::Type::FLOAT32_3:
      {
        int n =
          (propertyType==Ply::Element::Property::Type::FLOAT32_3)?3:
          (propertyType==Ply::Element::Property::Type::FLOAT32_2)?2:1;
        for(int i=0;i<n;i++) {
          svb.f[0] = (*static_cast<vector<float>*>(value))[n*UL(index)+i];
          if(swapBytes) Endian::swapFloat(svb);
          success = (fwrite(&(svb.f[0]),1,4,fp)==4);
          if(success==false) break;
        }
      }
      break;
    case Ply::Element::Property::Type::DOUBLE:
    case Ply::Element::Property::Type::FLOAT64:
      {
        svb.d[0] = (*static_cast<vector<double>*>(value))[UL(index)];
        if(swapBytes) Endian::swapDouble(svb);
        success = (fwrite(&(svb.d[0]),1,8,fp)==8);
      }
      break; 
    default:
      break;
    }
  }
  return success;
}

//////////////////////////////////////////////////////////////////////
// static
  
bool SaverPly::writeBinaryColorValue
(FILE * fp, const bool swapBytes, void* value, int index) {
  // TODO Thu Mar 2 2023
  // swapBytes unused???
  (void)swapBytes;
  bool success = false;
  if(fp!=nullptr) {
    Endian::SingleValueBuffer svb;
    for(int i=0;i<3;i++) {
      float& f = (*static_cast<vector<float>*>(value))[3*UL(index)+i]; 
      svb.uc[0] = static_cast<uchar>(255.0*f); 
      success = (fwrite(&(svb.uc[0]),1,1,fp)==1);
      if(success==false) break;
    }
  }
  return success;
}

//////////////////////////////////////////////////////////////////////
// static
bool SaverPly::writeAsciiValue
(FILE * fp, const Ply::Element::Property::Type propertyType,
 void* value, int index) {
  bool success = false;
  if(fp!=nullptr) {
    switch(propertyType) {
    case Ply::Element::Property::Type::CHAR:
    case Ply::Element::Property::Type::INT8:
      {
        char & c = (*static_cast<vector<char>*>(value))[UL(index)];
        success = (fprintf(fp,"%d",c)>0);
      }
      break;
    case Ply::Element::Property::Type::UCHAR:
    case Ply::Element::Property::Type::UINT8:
      {
        uchar & uc = (*static_cast<vector<uchar>*>(value))[UL(index)];
        success = (fprintf(fp,"%d",uc)>0);
      }
      break;
    case Ply::Element::Property::Type::SHORT:
    case Ply::Element::Property::Type::INT16:
      {
        short & s = (*static_cast<vector<short>*>(value))[UL(index)];
        success = (fprintf(fp,"%d",s)>0);
      }
      break;
    case Ply::Element::Property::Type::USHORT:
    case Ply::Element::Property::Type::UINT16:
      {
        ushort & us = (*static_cast<vector<ushort>*>(value))[UL(index)];
        success = (fprintf(fp,"%d",us)>0);
      }
      break;
    case Ply::Element::Property::Type::INT:
    case Ply::Element::Property::Type::INT32:
      {
        int & i = (*static_cast<vector<int>*>(value))[UL(index)];
        success = (fprintf(fp,"%d",i)>0);
      }
      break;
    case Ply::Element::Property::Type::UINT:
    case Ply::Element::Property::Type::UINT32:
      {
        uint & ui = (*static_cast<vector<uint>*>(value))[UL(index)];
        success = (fprintf(fp,"%d",ui)>0);
      }
      break;
    case Ply::Element::Property::Type::FLOAT:
    case Ply::Element::Property::Type::FLOAT32:
    case Ply::Element::Property::Type::FLOAT32_2:
    case Ply::Element::Property::Type::FLOAT32_3:
      {
        int n =
          (propertyType==Ply::Element::Property::Type::FLOAT32_3)?3:
          (propertyType==Ply::Element::Property::Type::FLOAT32_2)?2:1;
        for(int i=0;i<n;i++) {
          float & f = (*static_cast<vector<float>*>(value))[i+n*UL(index)];
          success = (fprintf(fp,"%f ",D(f))>0);
          if(success==false) break;
        }
      }
      break;
    case Ply::Element::Property::Type::DOUBLE:
    case Ply::Element::Property::Type::FLOAT64:
      {
        double & d = (*static_cast<vector<double>*>(value))[UL(index)];
        success = (fprintf(fp,"%f",d)>0);
      }
      break; 
    default:
      break;
    }
  }
  return success;
}

//////////////////////////////////////////////////////////////////////
// static
  
bool SaverPly::writeAsciiColorValue
(FILE * fp, void* value, int index) {
  bool success = false;
  if(fp!=nullptr) {
    for(int i=0;i<3;i++) {
      float& f = (*static_cast<vector<float>*>(value))[3*UL(index)+i]; 
      uchar uc = static_cast<uchar>(255.0*f); 
      success = (fprintf(fp,"%3d ",uc)>0);
      if(success==false) break;
    }
  }
  return success;
}

//////////////////////////////////////////////////////////////////////
// static
bool
SaverPly::writeHeader
(FILE *fp, Ply& ply, const string indent, Ply::DataType dataType) {

  bool success = false;

  if(_ostrm!=nullptr) {
    *_ostrm << indent << "SaverPly::writeHeader() {" << endl;
  }

  try {

    if(fp==nullptr) throw new StrException("fp==nullptr");

    uint i;
    int iElement,iProperty,nElements,nProperties,nRecords;
    Ply::Element*                element      = nullptr;
    Ply::Element::Property*      property     = nullptr;
    Ply::Element::Property::Type listType     = Ply::Element::Property::Type::NONE;
    Ply::Element::Property::Type propertyType = Ply::Element::Property::Type::NONE;
    string elementName, propertyName, line;

    line = "ply";
    if(_ostrm!=nullptr) {
      *_ostrm << indent << "  " << line << endl;
    }

    fprintf(fp,"%s\n",line.c_str());
    switch(dataType) {
    case Ply::DataType::ASCII:

      line = "format ascii 1.0";
      if(_ostrm!=nullptr) {
        *_ostrm << indent << "  " << line << endl;
      }

      fprintf(fp,"%s\n",line.c_str());
      break;
    case Ply::DataType::BINARY_BIG_ENDIAN:
      line = "format binary_big_endian 1.0";
      if(_ostrm!=nullptr) {
        *_ostrm << indent << "  " << line << endl;
      }
      fprintf(fp,"%s\n",line.c_str());
      break;
    case Ply::DataType::BINARY_LITTLE_ENDIAN:
      line = "format binary_little_endian 1.0";
      if(_ostrm!=nullptr) {
        *_ostrm << indent << "  " << line << endl;
      }
      fprintf(fp,"%s\n",line.c_str());
      break;
    case Ply::DataType::NONE:
      throw new StrException("ply format NONE");
    }

    fprintf(fp,"comment generated by DGP2025 from Ply\n");

    if(ply.getSkipComments()==false) {
      const vector<string>& comment = ply.getComments();
      for(i=0;i<comment.size();i++) {
        line = comment[i];
        // TODO Sat Nov 30 13:20:44 2023
        // if(line contains "generated") continue;
        if(line=="VCGLIB generated") continue;
        if(_ostrm!=nullptr) {
          *_ostrm << indent << "  " << line << endl;
        }
        fprintf(fp,"comment %s\n",line.c_str());
      }
    }

    if(ply.isTextured()) {
      fprintf(fp,"comment TextureFile %s\n",ply.getTextureFile().c_str());
    }

    nElements = ply.getNumberOfElements();
    for(iElement=0;iElement<nElements;iElement++) {

      element     = ply.getElement(iElement);
      elementName = element->getName();
      nRecords    = element->getNumberOfRecords();
      if(_ostrm!=nullptr) {
        *_ostrm << indent << "  element " << elementName << " "<< nRecords<< endl;
      }
      fprintf(fp,"element %s %d\n",elementName.c_str(),nRecords);

      nProperties = element->getNumberOfProperties();
      for(iProperty=0;iProperty<nProperties;iProperty++) {
        property = element->getProperty(iProperty);
        propertyName = property->getName();
        if(_skipAlpha && propertyName=="alpha") continue;

        propertyType = property->getPropertyType();

        if(property->isList()) {

          if(propertyName=="coordIndex") {

            if(_ostrm!=nullptr) {
              *_ostrm << indent
                      << "  property list uchar int vertex_indices" << endl;
            }
            fprintf(fp,"property list uchar int vertex_indices\n");

          } else {

            listType = property->getListType();
            if(_ostrm!=nullptr) {
              *_ostrm << indent << "  property list "
                      << Ply::Element::Property::getTypeName(listType) << " "
                      << Ply::Element::Property::getTypeName(propertyType) << " "
                      << propertyName << endl;
            }
            fprintf(fp,"property list %s %s %s\n",
                    Ply::Element::Property::getTypeName(listType).c_str(),
                    Ply::Element::Property::getTypeName(propertyType).c_str(),
                    propertyName.c_str());

          }

        } else /* if(property->isList()==false) */ {

          if(propertyName=="coord") {

            if(_ostrm!=nullptr) {
              *_ostrm << indent << "  property float x" << endl;
              *_ostrm << indent << "  property float y" << endl;
              *_ostrm << indent << "  property float z" << endl;
            }
            fprintf(fp,"property float x\n");
            fprintf(fp,"property float y\n");
            fprintf(fp,"property float z\n");

          } else if(propertyName=="normal") {

            if(_ostrm!=nullptr) {
              *_ostrm << indent << "  property float nx" << endl;
              *_ostrm << indent << "  property float ny" << endl;
              *_ostrm << indent << "  property float nz" << endl;
            }
            fprintf(fp,"property float nx\n");
            fprintf(fp,"property float ny\n");
            fprintf(fp,"property float nz\n");

          } else if(propertyName=="color") {

            if(_ostrm!=nullptr) {
              *_ostrm << indent << "  property uchar red" << endl;
              *_ostrm << indent << "  property uchar green" << endl;
              *_ostrm << indent << "  property uchar blue" << endl;
            }
            fprintf(fp,"property uchar red\n");
            fprintf(fp,"property uchar green\n");
            fprintf(fp,"property uchar blue\n");

          } else {
            if(_ostrm!=nullptr) {
              *_ostrm << indent << "  property "
                      << Ply::Element::Property::getTypeName(propertyType) << " "
                      << propertyName << endl;
            }
            fprintf(fp,"property %s %s\n",
                    Ply::Element::Property::getTypeName(propertyType).c_str(),
                    propertyName.c_str());
          
          }

        }

      } // for(iProperty=0;iProperty<nProperties;iProperty++) ...

    } // for(iElement=0;iElement<nElements;iElement++) ...

    line = "end_header";
    if(_ostrm!=nullptr) {
      *_ostrm << indent << "  " << line<< endl;
    }
    fprintf(fp,"%s\n",line.c_str());

    success = true;

  } catch (StrException* e) {
    if(_ostrm!=nullptr) {
      *_ostrm << indent << "  " << e->what()<< endl;
    }
    delete e;
  }

  if(_ostrm!=nullptr) {
    *_ostrm << indent << "} SaverPly::writeHeader()" << endl;
  }

  return success;
}

//////////////////////////////////////////////////////////////////////
// static
bool
SaverPly::writeBinaryData
(FILE * fp, Ply& ply, const string indent, Ply::DataType dataType) {

  bool success = false;

  if(_ostrm!=nullptr) {
    *_ostrm << indent << "SaverPly::writeBinaryData() {" << endl;
  }

  try {

    bool swapBytes = (sameAsSystemEndian(dataType)==false);

    Ply::Element* element;
    Ply::Element::Property* property;
    Ply::Element::Property::Type listType;
    Ply::Element::Property::Type propertyType;
    void* propertyValue;
    int iElement,iList0,iList1,iList,nList,k0,k1;
    int iProperty,iRecord,nElements,nProperties,nRecords;
    string name,propertyName;

    nElements = ply.getNumberOfElements();
    if(_ostrm!=nullptr) {
      *_ostrm << indent << "  nElements " << nElements << endl;
    }

    for(iElement=0;iElement<nElements;iElement++) {
      element     = ply.getElement(iElement);
      name        = element->getName();
      if(_ostrm!=nullptr) {
        *_ostrm << indent << "    name " << name << endl;
      }

      nProperties = element->getNumberOfProperties();
      if(_skipAlpha && element->hasProperty("alpha")) nProperties--;
      if(_ostrm!=nullptr) {
        *_ostrm << indent << "      nProperties = " << nProperties<< endl;
      }

      nRecords    = element->getNumberOfRecords();
      if(_ostrm!=nullptr) {
        *_ostrm << indent << "      nRecords = " << nRecords << endl;
        *_ostrm << indent << "        ";
      }

      for(k0=iRecord=0;iRecord<nRecords;iRecord++) {

        for(iProperty=0;iProperty<nProperties;iProperty++) {
          property      = element->getProperty(iProperty);
          propertyName  = property->getName();
          if(_skipAlpha && propertyName=="alpha") continue;
          propertyType  = property->getPropertyType();
          propertyValue = property->getValue();

          if(property->isList()) {
            listType = property->getListType();
            iList0   = property->getListFirst(iRecord );
            nList    = property->getListFirst(iRecord+1)-iList0;
            if(propertyName=="coordIndex") nList--; // don't write -1 separator
            iList1   = iList0+nList;
            
            if(writeBinaryValue(fp,listType,swapBytes,nList)==false)
              throw new StrException("unable to write list binary count");
            for(iList=iList0;iList<iList1;iList++) {
              if(writeBinaryValue
                 (fp,propertyType,swapBytes,propertyValue,iList)==false)
                throw new StrException("unable to write list binary value");
            }
          } else {
            if(propertyName=="color") {
              if(writeBinaryColorValue(fp,swapBytes,propertyValue,iRecord)==false)
                throw new StrException("unable to write binary value");
            } else {
              if(writeBinaryValue
                 (fp,propertyType,swapBytes,propertyValue,iRecord)==false)
                throw new StrException("unable to write binary value");
            }
          }
          
        } // for(iProperty ...

        // report progress
        k1 = (10*(iRecord+1))/nRecords;
        if(k1>k0) {
          if(_ostrm!=nullptr) {
            *_ostrm << (10*k1) << "% ";
          }
          k0 = k1;
        }
          
      } // for(iRecord ...
      
      if(_ostrm!=nullptr) {
        *_ostrm << endl;
      }
        
    }
      
    success = true;
      
  } catch (StrException* e) {
    if(_ostrm!=nullptr) {
      *_ostrm << indent << "  " << e->what() << endl;
    }
    delete e;
  }
    
  if(_ostrm!=nullptr) {
    *_ostrm << indent << "} SaverPly::writeBinaryData()" << endl;
  }
    
  return success;
}

//////////////////////////////////////////////////////////////////////
// static
bool
SaverPly::writeAsciiData
(FILE * fp, Ply& ply, const string indent,Ply::DataType dataType) {

  bool success = false;

  if(_ostrm!=nullptr) {
    *_ostrm << indent << "  SaverPly::writeAsciiData() {" << endl;
  }

  try {

    if(dataType!=Ply::DataType::ASCII)
        throw new StrException("  incorrect data type");

    Ply::Element* element;
    Ply::Element::Property* property;
    // Ply::Element::Property::Type listType;
    Ply::Element::Property::Type propertyType;
    void* propertyValue;
    int iElement,iList0,iList1,iList,nList,iProperty;
    int iRecord,nElements,nProperties,nRecords,k0,k1;
    string name,propertyName;

    nElements = ply.getNumberOfElements();
    if(_ostrm!=nullptr) {
      *_ostrm << indent << "  nElements = " << nElements << endl;
    }

    for(iElement=0;iElement<nElements;iElement++) {
      element     = ply.getElement(iElement);
      name        = element->getName();
      if(_ostrm!=nullptr) {
        *_ostrm << indent << "  name = " << name << endl;
      }

      nProperties = element->getNumberOfProperties();
      if(_skipAlpha && element->hasProperty("alpha")) nProperties--;
      if(_ostrm!=nullptr) {
        *_ostrm << indent << "      nProperties = " << nProperties << endl;
      }

      nRecords    = element->getNumberOfRecords();
      if(_ostrm!=nullptr) {
        *_ostrm << indent << "      nRecords = " << nRecords << endl;
        *_ostrm << indent << "        ";
      }

      for(k0=iRecord=0;iRecord<nRecords;iRecord++) {

        for(iProperty=0;iProperty<nProperties;iProperty++) {
          property      = element->getProperty(iProperty);
          propertyName  = property->getName();
          if(_skipAlpha && propertyName=="alpha") continue;
          propertyType  = property->getPropertyType();
          propertyValue = property->getValue();

          if(property->isList()) {
            // listType = property->getListType();
            iList0   = property->getListFirst(iRecord );
            nList    = property->getListFirst(iRecord+1)-iList0;
            if(propertyName=="coordIndex") nList--; // don't write -1 separator 
            iList1   = iList0+nList;

            fprintf(fp,"%d ",nList);
            for(iList=iList0;iList<iList1;) {
              if(writeAsciiValue(fp,propertyType,propertyValue,iList)==false)
                throw new StrException("unable to write list ascii value");
              if(++iList<iList1) fprintf(fp," ");
            }

          } else /* if(property->isList()==false) */ {
            if(propertyName=="color") {
              if(writeAsciiColorValue(fp,propertyValue,iRecord)==false)
                throw new StrException("unable to write ascii color value");
            } else {
              if(writeAsciiValue(fp,propertyType,propertyValue,iRecord)==false)
                throw new StrException("unable to write ascii value");
            }

            fprintf(fp," ");
          }
        }
        fprintf(fp,"\n"); // end of record

        k1 = (10*(iRecord+1))/nRecords;
        if(k1>k0) {
          if(_ostrm!=nullptr) {
            *_ostrm << (10*k1) << "% ";
          }
          k0 = k1;
        }
      }
      if(_ostrm!=nullptr) {
        *_ostrm << endl;
      }

    }

  } catch (StrException* e) {
    if(_ostrm!=nullptr) {
      *_ostrm << indent << "  " << e->what() << endl;
    }
    delete e;
  }

  if(_ostrm!=nullptr) {
    *_ostrm << indent << "} SaverPly::writeAsciiData()" << endl;
  }
  
  return success;
}

//////////////////////////////////////////////////////////////////////
// static
bool SaverPly::save
(const char* filename, Ply & ply, const string indent, Ply::DataType dataType) {

  bool success = false;

  if(_ostrm!=nullptr) {
    *_ostrm << indent << "SaverPly::save(Ply &) {" << endl;
    *_ostrm << indent << "  filename = " << endl;
    *_ostrm << indent << "  \"" << filename << "\"" << endl;
  }

  FILE* fp  = nullptr;
      
  try {
        
    if(filename==nullptr) throw new StrException("filename==nullptr");

    fp = fopen(filename,"w");
    if(fp==nullptr) throw new StrException("fp==nullptr");


    if(writeHeader(fp,ply,indent+"  ",dataType)==false)
      throw new StrException("unable to write file header");

    if(dataType==Ply::DataType::NONE)
      throw new StrException("ply DataType is NONE");
    else if(dataType==Ply::DataType::ASCII) {
      if(writeAsciiData(fp,ply,indent+"  ")==false)
        throw new StrException("unable to write ASCII data");
    } else /* if(dataType==Ply::DataType::BINARY) */ {
      // close file;
      fflush(fp);
      fclose(fp);
      // reopen file for binary append
      fp = fopen(filename,"ab");
      if(writeBinaryData(fp,ply,indent+"  ",dataType)==false)
        throw new StrException("unable to write BINARY data");
    }

    fclose(fp);
    success = true;

  } catch(StrException* e) { 
    if(fp) { fclose(fp); fp = nullptr; }
    if(_ostrm!=nullptr) {
      *_ostrm << indent << "  " << e->what() << endl;
    }
  
    delete e;
  }

  if(_ostrm!=nullptr) {
    *_ostrm << indent << "} SaverPly::save(Ply &)" << endl;
  }

  return success;
}

//////////////////////////////////////////////////////////////////////
// static
bool SaverPly::writeHeader
(FILE *fp, IndexedFaceSet& ifs, const string indent, Ply::DataType dataType) { 

  bool success = false;

  if(_ostrm!=nullptr) {
    *_ostrm << indent << "SaverPly::writeHeader(IndexedFaceSet &) {" << endl;
  }

  try {

    if(fp==nullptr)
      throw new StrException("fp==nullptr");

    if(ifs.hasVertices()==false)
      throw new StrException("ifs does not have vertices");

    if(ifs.hasColorPerCorner()  ||
       ifs.hasNormalPerCorner() ||
       ifs.hasTexCoordPerCorner())
      throw new StrException("ifs properties per corner not supported yet");

    string line = "ply";
    if(_ostrm!=nullptr) {
      *_ostrm << indent << "  " << line << endl;
    }

    fprintf(fp,"%s\n",line.c_str());
    switch(dataType) {
    case Ply::DataType::ASCII:
      line = "format ascii 1.0";
      if(_ostrm!=nullptr) {
        *_ostrm << indent << "  " << line << endl;
      }
      fprintf(fp,"%s\n",line.c_str());
      break;
    case Ply::DataType::BINARY_BIG_ENDIAN:
      line = "format binary_big_endian 1.0";
      if(_ostrm!=nullptr) {
        *_ostrm << indent << "  " << line << endl;
      }
      fprintf(fp,"%s\n",line.c_str());
      break;
    case Ply::DataType::BINARY_LITTLE_ENDIAN:
      line = "format binary_little_endian 1.0";
      if(_ostrm!=nullptr) {
        *_ostrm << indent << "  " << line << endl;
      }
      fprintf(fp,"%s\n",line.c_str());
      break;
    case Ply::DataType::NONE:
      throw new StrException("ply format NONE");
    }

    fprintf(fp,"comment generated DGP2025 by from IndexedFaceSet\n");

    // TODO Fri Mar  3 11:59:43 2023
    // - get access to containing Shape->appearance->ImageTexture url

    // if(ifs.isTextured()) {
    //   fprintf(fp,"comment TextureFile %s\n",ply.getTextureFile().c_str());
    // }

    // vertices
    int nVertices = ifs.getNumberOfVertices();
    fprintf(fp,"element vertex %d\n",nVertices);    

    // coord -> FLOAT x,y,z
    fprintf(fp,"property float x\n");
    fprintf(fp,"property float y\n");
    fprintf(fp,"property float z\n");

    // normal -> FLOAT nx,ny,nz
    if(ifs.hasNormalPerVertex()) {
      fprintf(fp,"property float nx\n");
      fprintf(fp,"property float ny\n");
      fprintf(fp,"property float nz\n");
    }

    // color -> UCHAR red,green,blue
    if(ifs.hasColorPerVertex()) {
      fprintf(fp,"property uchar red\n");
      fprintf(fp,"property uchar green\n");
      fprintf(fp,"property uchar blue\n");
    }

    // texCoord -> FLOAT u,v
    if(ifs.hasTexCoordPerVertex()) {
      fprintf(fp,"property float u\n");
      fprintf(fp,"property float v\n");
    }

    if(ifs.hasFaces()) {
      int nFaces = ifs.getNumberOfFaces();
      fprintf(fp,"element face %d\n",nFaces);    

      // coordIndex ->vertex_indices
      fprintf(fp,"property list uchar int vertex_indices\n");

      // normal -> FLOAT nx,ny,nz
      if(ifs.hasNormalPerFace()) {
        fprintf(fp,"property float nx\n");
        fprintf(fp,"property float ny\n");
        fprintf(fp,"property float nz\n");
      }
          
      // color -> UCHAR red,green,blue
      if(ifs.hasColorPerVertex()) {
        fprintf(fp,"property uchar red\n");
        fprintf(fp,"property uchar green\n");
        fprintf(fp,"property uchar blue\n");            
      }

      // TODO Fri Mar  3 11:59:55 2023
      // color, normal, texCoord per corner
    }

    line = "end_header";
    if(_ostrm!=nullptr) {
      *_ostrm << indent << "  " << line << endl;
    }
    fprintf(fp,"%s\n",line.c_str());

    success = true;

  } catch (StrException* e) {
    if(_ostrm!=nullptr) {
      *_ostrm << indent << "  " << e->what()<< endl;
    }
    delete e;
  }

  if(_ostrm!=nullptr) {
    *_ostrm << indent << "} SaverPly::writeHeader(IndexedFaceSet &)" << endl;
  }

  return success;
}

//////////////////////////////////////////////////////////////////////
// static
bool
SaverPly::writeBinaryData
(FILE * fp, IndexedFaceSet& ifs, const string indent, Ply::DataType dataType) {

  if(_ostrm!=nullptr) {
    *_ostrm << indent << "SaverPly::writeBinaryData(IndexedFaceSet &) {" << endl;
  }

  if(fp==nullptr              ||
     ifs.hasVertices()==false ||
     ifs.hasColorPerCorner()  ||
     ifs.hasNormalPerCorner() ||
     ifs.hasTexCoordPerCorner()) {
    if(_ostrm!=nullptr) {
      *_ostrm << indent << "  ERROR" << endl;
      *_ostrm << indent << "} SaverPly::writeBinaryData(IndexedFaceSet &)" << endl;
    }
    return false;
  }

  bool swapBytes = (sameAsSystemEndian(dataType)==false);

  int i,i0,i1,iF,nList,iV,iN,iC,j,k0,k1;

  vector<float>& coord         = ifs.getCoord();
  vector<int>&   coordIndex    = ifs.getCoordIndex();
  vector<float>& normal        = ifs.getNormal();
  vector<int>&   normalIndex   = ifs.getNormalIndex();
  vector<float>& color         = ifs.getColor();
  vector<int>&   colorIndex    = ifs.getColorIndex();
  vector<float>& texCoord      = ifs.getTexCoord();
  // vector<int>&   texCoordIndex = ifs.getTexCoordIndex();

  int nVertices = ifs.getNumberOfVertices();
  int nFaces    = ifs.getNumberOfFaces();

  Endian::SingleValueBuffer svb;

  if(_ostrm!=nullptr) {
    *_ostrm << indent << "  name = vertex" << endl;
    *_ostrm << indent << "    ";
  }

  for(k0=iV=0;iV<nVertices;iV++) {

    if(true /* ifs.hasCoordPerVertex() */) {
      for(j=0;j<3;j++) {
        svb.f[0] = coord[UI(3*iV+j)];
        if(swapBytes) Endian::swapFloat(svb);
        fwrite(&(svb.f[0]),1,4,fp); // ==4
      }
    }
    if(ifs.hasNormalPerVertex()) {
      for(j=0;j<3;j++) {
        svb.f[0] = normal[UI(3*iV+j)];
        if(swapBytes) Endian::swapFloat(svb);
        fwrite(&(svb.f[0]),1,4,fp); // ==4
      }
    }
    if(ifs.hasColorPerVertex()) {
      for(j=0;j<3;j++) {
        svb.uc[0] = UC(color[UI(3*iV+j)]*255.0f);
        if(swapBytes) Endian::swapFloat(svb);
        fwrite(&(svb.uc[0]),1,1,fp); // ==1
      }
    }
    if(ifs.hasTexCoordPerVertex()) {
      for(j=0;j<2;j++) {
        svb.f[0] = texCoord[UI(2*iV+j)];
        if(swapBytes) Endian::swapFloat(svb);
        fwrite(&(svb.f[0]),1,4,fp); // ==4
      }
    }

    k1 = (10*(iV+1))/nVertices;
    if(k1>k0) {
      if(_ostrm!=nullptr) {
        *_ostrm << (10*k1) << "% ";
      }
      k0 = k1;
    }
  }
  if(_ostrm!=nullptr) {
    *_ostrm << endl;
  }

  if(nFaces>0) {
    if(_ostrm!=nullptr) {
      *_ostrm << indent << "  name = face" << endl;
      *_ostrm << indent << "    ";
    }

    bool ifsHasNormalPerFace = ifs.hasNormalPerFace();
    bool ifsHasColorPerFace  = ifs.hasColorPerFace();

    for(k0=iF=i0=i1=0;i1<I(coordIndex.size());i1++) {

      if(coordIndex[UI(i1)]<0) {
        nList = i1-i0;

        svb.uc[0] = UC(nList);
        fwrite(&(svb.uc[0]),1,1,fp); // ==1

        for(i=i0;i<i1;i++) {
          svb.i[0] = coordIndex[UI(i)];
          if(swapBytes) Endian::swapInt(svb);
          fwrite(&(svb.i[0]),1,4,fp); // ==4
        }

        if(ifsHasNormalPerFace) {
          iN = (normalIndex.size()>0)?normalIndex[UI(iF)]:iF;
          for(j=0;j<3;j++) {
            svb.f[0] = normal[UI(3*iN+j)];
            if(swapBytes) Endian::swapFloat(svb);
            fwrite(&(svb.f[0]),1,4,fp); // ==4
          }
        }

        if(ifsHasColorPerFace) {
          iC = (colorIndex.size()>0)?colorIndex[UI(iF)]:iF;
          for(j=0;j<3;j++) {
            svb.uc[0] = UC(color[UI(3*iC+j)]*255.0f);
            if(swapBytes) Endian::swapFloat(svb);
            fwrite(&(svb.uc[0]),1,1,fp); // ==1
          }
        }

        k1 = (10*(iF+1))/nFaces;
        if(k1>k0) {
          if(_ostrm!=nullptr) {
            *_ostrm << (10*k1) << "% ";
          }
          k0 = k1;
        }

        i0=i1+1; iF++;
      }
    }

    if(_ostrm!=nullptr) {
      *_ostrm << endl;
    }
    
  } // if(nFaces>0)

  if(_ostrm!=nullptr) {
    *_ostrm << indent << "} SaverPly::writeBinaryData(IndexedFaceSet &)" << endl;
  }

  return true;
}

//////////////////////////////////////////////////////////////////////
// static
bool
SaverPly::writeAsciiData
(FILE * fp, IndexedFaceSet& ifs, const string indent, Ply::DataType dataType) {

  if(_ostrm!=nullptr) {
    *_ostrm << indent << "SaverPly::writeAsciiData(IndexedFaceSet &) {" << endl;
  }

  if(fp==nullptr                    ||
     dataType!=Ply::DataType::ASCII ||
     ifs.hasVertices()==false       ||
     ifs.hasColorPerCorner()        ||
     ifs.hasNormalPerCorner()       ||
     ifs.hasTexCoordPerCorner()     ) {
    if(_ostrm!=nullptr) {
      *_ostrm << indent << "  ERROR" << endl;
      *_ostrm << indent << "} SaverPly::writeAsciiData(IndexedFaceSet &)" << endl;
    }
    return false;
  }

  int i,i0,i1,iF,iV,iN,iC,j,k0,k1;
  uint nList;

  vector<float>& coord         = ifs.getCoord();
  vector<int>&   coordIndex    = ifs.getCoordIndex();
  vector<float>& normal        = ifs.getNormal();
  vector<int>&   normalIndex   = ifs.getNormalIndex();
  vector<float>& color         = ifs.getColor();
  vector<int>&   colorIndex    = ifs.getColorIndex();
  vector<float>& texCoord      = ifs.getTexCoord();
  // vector<int>&   texCoordIndex = ifs.getTexCoordIndex();

  int nVertices = ifs.getNumberOfVertices();
  int nFaces    = ifs.getNumberOfFaces();

  if(_ostrm!=nullptr) {
    *_ostrm << indent << "  name = vertex" << endl;
    *_ostrm << indent << "    ";
  }

  for(k0=iV=0;iV<nVertices;iV++) {

    if(true /* ifs.hasCoordPerVertex() */) {
      for(j=0;j<3;j++)
        fprintf(fp,"%f ",D(coord[UI(3*iV+j)]));
    }
    if(ifs.hasNormalPerVertex()) {
      for(j=0;j<3;j++)
        fprintf(fp,"%f ",D(normal[UI(3*iV+j)]));
    }
    if(ifs.hasColorPerVertex()) {
      for(j=0;j<3;j++)
        fprintf(fp,"%d ",UC(color[UI(3*iV+j)]*255.0f));
    }
    if(ifs.hasTexCoordPerVertex()) {
      for(j=0;j<2;j++)
        fprintf(fp,"%f ",D(texCoord[UI(2*iV+j)]));
    }
    fprintf(fp,"\n");

    k1 = (10*(iV+1))/nVertices;
    if(k1>k0) {
      if(_ostrm!=nullptr) {
        *_ostrm << (10*k1) << "% ";
      }
      k0 = k1;
    }
  }
  if(_ostrm!=nullptr) {
    *_ostrm << endl;
  }
  
  if(nFaces>0) {
    if(_ostrm!=nullptr) {
      *_ostrm << indent << "  name = face" << endl;
      *_ostrm << indent << "    ";
    }

    bool ifsHasNormalPerFace = ifs.hasNormalPerFace();
    bool ifsHasColorPerFace  = ifs.hasColorPerFace();

    for(k0=iF=i0=i1=0;i1<I(coordIndex.size());i1++) {
      if(coordIndex[UI(i1)]<0) {
        nList = UC(i1-i0);

        fprintf(fp,"%d ",nList);
        for(i=i0;i<i1;i++)
          fprintf(fp,"%d ",coordIndex[UI(i)]);
        
        if(ifsHasNormalPerFace) {
          iN = (normalIndex.size()>0)?normalIndex[UI(iF)]:iF;
          for(j=0;j<3;j++)
            fprintf(fp,"%f ",D(normal[UI(iN)]));
        }

        if(ifsHasColorPerFace) {
          iC = (colorIndex.size()>0)?colorIndex[UI(iF)]:iF;
          for(j=0;j<3;j++)
            fprintf(fp,"%f ",D(color[UC(iC)]));
        }

        fprintf(fp,"\n");

        k1 = (10*(iF+1))/nFaces;
        if(k1>k0) {
          if(_ostrm!=nullptr) {
            *_ostrm << (10*k1) << "% ";
          }
          k0 = k1;
        }

        i0=i1+1; iF++;
      }
    }
    if(_ostrm!=nullptr) {
      *_ostrm << endl;
    }
  } // if(nFaces>0)

  if(_ostrm!=nullptr) {
    *_ostrm << indent << "} SaverPly::writeAsciiData(IndexedFaceSet &)" << endl;
  }
  return true;
}

//////////////////////////////////////////////////////////////////////
// static
bool
SaverPly::save
(const char* filename, IndexedFaceSet & ifs, const string indent,
 Ply::DataType dataType) {

  bool success = false;

  if(_ostrm!=nullptr) {
    *_ostrm << indent << "SaverPly::save(IndexedFaceSet &) {" << endl;
    *_ostrm << indent << "  filename =" << endl;
    *_ostrm << indent << "  \"" << filename << "\"" << endl;
  }


  FILE* fp  = nullptr;
      
  try {
        
    if(filename==nullptr) throw new StrException("filename==nullptr");

    fp = fopen(filename,"w");
    if(fp==nullptr) throw new StrException("fp==nullptr");

    if(writeHeader(fp,ifs,indent+"  ",dataType)==false)
      throw new StrException("unable to write file header");

    if(dataType==Ply::DataType::NONE)
      throw new StrException("ifs DataType is NONE");
    else if(dataType==Ply::DataType::ASCII) {
      if(writeAsciiData(fp,ifs,indent+"  ",dataType)==false)
        throw new StrException("unable to write ASCII data");
    } else /* if(dataType==Ply::DataType::BINARY) */ {
      // close file;
      fflush(fp);
      fclose(fp);
      // reopen file for binary append
      fp = fopen(filename,"ab");
      if(writeBinaryData(fp,ifs,indent+"  ",dataType)==false)
        throw new StrException("unable to write BINARY data");
    }

    fclose(fp);
    success = true;

  } catch(StrException* e) { 
    if(fp) { fclose(fp); fp = nullptr; }
    if(_ostrm!=nullptr) {
      *_ostrm << indent << "  " << e->what() << endl;
    }
    delete e;
  }

  if(_ostrm!=nullptr) {
    *_ostrm << indent << "} SaverPly::save(IndexedFaceSet &)" << endl;
  }

  return success;
}

//////////////////////////////////////////////////////////////////////
// virtual
bool
SaverPly::save(const char* filename, SceneGraph& wrl) const {

  const string indent = _indent;
  
  bool success = false;

  if(_ostrm!=nullptr) {
    *_ostrm << indent << "SaverPly::save(SceneGraph &) {" << endl;
  }

  try {
    if(filename==nullptr)  throw new StrException("filename==nullptr");

    if(wrl.getNumberOfChildren()!=1)
      throw new StrException("wrl.getNumberOfChildren()!=1");

    Node* node = wrl[0];
    Shape* shape = dynamic_cast<Shape*>(node);
    if(shape==nullptr) throw new StrException("shape==nullptr");
    node = shape->getAppearance();
    Appearance* appearance = dynamic_cast<Appearance*>(node);
    if(appearance==nullptr) throw new StrException("appearance==nullptr");
    node = appearance->getMaterial();
    Material* material = dynamic_cast<Material*>(node);
    if(material==nullptr) throw new StrException("material==nullptr");

    // TODO Fri Feb 17 19:35:17 2023
    // save diffuseColor in the ply file
    //
    // const Color& diffuseColor = material->getDiffuseColor();

    node = shape->getGeometry();
    if(IndexedFaceSetPly* ifsPly = dynamic_cast<IndexedFaceSetPly*>(node)) {

      Ply* ply = ifsPly->getPly();
      if(ply==nullptr) throw new StrException("ply==nullptr");

      if(save(filename,*ply,indent+"  ",_dataType)==false)
        throw new StrException("save(fp,Ply&)==false");
    
      success = true;

    } else if(IndexedFaceSet* ifs = dynamic_cast<IndexedFaceSet*>(node)) {

      if(save(filename,*ifs,indent+"  ",_dataType)==false)
        throw new StrException("save(fp,IndexedFaceSet&)==false");
    
      success = true;

    } else /* if(node==nullptr) */ {
      throw new StrException("shape->geometry == nullptr or invalid");
    }

  } catch(StrException* e) { 
    if(_ostrm!=nullptr) {
      *_ostrm << indent << "  " << e->what()<< endl;
    }
    delete e;
  }

  if(_ostrm!=nullptr) {
    *_ostrm << indent << "} SaverPly::save(SceneGraph &)" << endl;
  }

  return success;
}
