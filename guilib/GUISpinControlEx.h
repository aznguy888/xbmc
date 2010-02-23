/*!
\file GUISpinControlEx.h
\brief
*/

#ifndef GUILIB_SPINCONTROLEX_H
#define GUILIB_SPINCONTROLEX_H

#pragma once

/*
 *      Copyright (C) 2005-2008 Team XBMC
 *      http://www.xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include "GUISpinControl.h"
#include "GUIButtonControl.h"

/*!
 \ingroup controls
 \brief
 */
class CGUISpinControlEx : public CGUISpinControl
{
public:
  CGUISpinControlEx(int parentID, int controlID, float posX, float posY, float width, float height, float spinWidth, float spinHeight, const CLabelInfo& spinInfo, const CTextureInfo &textureFocus, const CTextureInfo &textureNoFocus, const CTextureInfo& textureUp, const CTextureInfo& textureDown, const CTextureInfo& textureUpFocus, const CTextureInfo& textureDownFocus, const CLabelInfo& labelInfo, int iType);
  virtual ~CGUISpinControlEx(void);
  virtual CGUISpinControlEx *Clone() const { return new CGUISpinControlEx(*this); };

  virtual void Render();
  virtual void SetPosition(float posX, float posY);
  virtual float GetWidth() const { return m_buttonControl.GetWidth();};
  virtual void SetWidth(float width);
  virtual float GetHeight() const { return m_buttonControl.GetHeight();};
  virtual void SetHeight(float height);
  virtual void AllocResources();
  virtual void FreeResources();
  virtual void DynamicResourceAlloc(bool bOnOff);
  virtual void SetInvalid();
  const CStdString GetCurrentLabel() const;
  void SetText(const std::string & aLabel) {m_buttonControl.SetLabel(aLabel);};
  virtual void SetVisible(bool bVisible);
  virtual void SetEnabled(bool bEnable);
  virtual float GetXPosition() const { return m_buttonControl.GetXPosition();};
  virtual float GetYPosition() const { return m_buttonControl.GetYPosition();};
  virtual CStdString GetDescription() const;
  virtual bool HitTest(const CPoint &point) const { return m_buttonControl.HitTest(point); };
  void SetSpinPosition(float spinPosX);

  void SetItemInvalid(bool invalid);
protected:
  virtual void RenderText(float posX, float width);
  virtual void UpdateColors();
  CGUIButtonControl m_buttonControl;
  float m_spinPosX;
};
#endif
