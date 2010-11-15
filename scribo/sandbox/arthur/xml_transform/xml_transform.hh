// Copyright (C) 2010 EPITA Research and Development Laboratory (LRDE)
//
// This file is part of Olena.
//
// Olena is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation, version 2 of the License.
//
// Olena is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Olena.  If not, see <http://www.gnu.org/licenses/>.
//
// As a special exception, you may use this file as part of a free
// software project without restriction.  Specifically, if other files
// instantiate templates or use macros or inline functions from this
// file, or you compile this file and link it with other files to produce
// an executable, this file does not by itself cause the resulting
// executable to be covered by the GNU General Public License.  This
// exception does not however invalidate any other reasons why the
// executable file might be covered by the GNU General Public License.

#ifndef XML_TRANFORM_HH
# define XML_TRANFORM_HH

# include <QtCore>


class Loader;
class ImageCrop;

class XmlTransform : public QObject
{
  Q_OBJECT
  public:

  XmlTransform(const QString& xml_file, const QString& image_file,
	       const QString& output, const QString& file = QString::Null());
  ~XmlTransform();

  QString out() { return output_dir_; }
  void createHTML(bool base64);
  void createPDF(bool crop, bool base64);
  void createSVG();
  void createOpen();
  void toBase64(bool crop);
  void fromBase64();

private:
  QString output_dir_;
  QString xml_file_;
  QString image_;
  Loader* loader_;
  ImageCrop* crop_;
  QString file_;
  bool tmp_;
};

#endif // !XML_TRANFORM_HH
