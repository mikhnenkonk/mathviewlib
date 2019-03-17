// Copyright (C) 2000-2007, Luca Padovani <padovani@sti.uniurb.it>.
//
// This file is part of GtkMathView, a flexible, high-quality rendering
// engine for MathML documents.
// 
// GtkMathView is free software; you can redistribute it and/or modify it
// either under the terms of the GNU Lesser General Public License version
// 3 as published by the Free Software Foundation (the "LGPL") or, at your
// option, under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation (the "GPL").  If you do not
// alter this notice, a recipient may use your version of this file under
// either the GPL or the LGPL.
//
// GtkMathView is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the LGPL or
// the GPL for more details.
// 
// You should have received a copy of the LGPL and of the GPL along with
// this program in the files COPYING-LGPL-3 and COPYING-GPL-2; if not, see
// <http://www.gnu.org/licenses/>.

#include <config.h>

#include <cassert>

#include "AreaFactory.hh"
#include "FormattingContext.hh"
#include "MathGraphicDevice.hh"
#include "MathMLDummyElement.hh"
#include "MathMLFractionElement.hh"
#include "MathMLOperatorElement.hh"
#include "ValueConversion.hh"
#include "MathMLAttributeSignatures.hh"

#include <iostream>

MathMLFractionElement::MathMLFractionElement(const SmartPtr<class MathMLNamespaceContext>& context)
  : MathMLContainerElement(context)
{ 
    std::cout << "[MathMLFractionElement]:MathMLFractionElement // creating MathMLFractionElement element" << std::endl;
}

MathMLFractionElement::~MathMLFractionElement()
{ }

AreaRef
MathMLFractionElement::format(FormattingContext& ctxt)
{
  if (dirtyLayout())
    {
      Length thickness;
      if (SmartPtr<Value> value = GET_ATTRIBUTE_VALUE(MathML, Fraction, linethickness)) // get fraction attribute from libxml2 element initially
	{
	  if (IsTokenId(value))
	    {
	      switch (ToTokenId(value))
		{
		case T_THIN:
		  thickness.set(0.5, Length::PURE_UNIT);
		  break;
		case T_MEDIUM:
		  thickness.set(1, Length::PURE_UNIT);
		  break;
		case T_THICK:
		  thickness.set(2, Length::PURE_UNIT);
		  break;
		default:
		  assert(false);
		  break;
		}
	    }
	  else if (IsNumber(value))
	    thickness.set(ToNumber(value), Length::PURE_UNIT);
	  else
	    thickness = ToLength(value);
	}
      else
	assert(false);

      TokenId numAlign = ToTokenId(GET_ATTRIBUTE_VALUE(MathML, Fraction, numalign));
      TokenId denomAlign = ToTokenId(GET_ATTRIBUTE_VALUE(MathML, Fraction, denomalign));
      bool bevelled = ToBoolean(GET_ATTRIBUTE_VALUE(MathML, Fraction, bevelled));

      AreaRef res;

      ctxt.push(this);
      ctxt.push(this);
      if (ctxt.getDisplayStyle()) ctxt.setDisplayStyle(false);
      else ctxt.addScriptLevel(1);
      AreaRef num = getNumerator()->format(ctxt);
      AreaRef denom = getDenominator()->format(ctxt);
      ctxt.pop();

      if (bevelled)
	res = ctxt.MGD()->bevelledFraction(ctxt, num, denom, thickness);
      else
	{
	  switch (numAlign)
	    {
	    case T_LEFT: break; // nothing to do
	    case T_CENTER: num = ctxt.MGD()->getFactory()->center(num); break;
	    case T_RIGHT: num = ctxt.MGD()->getFactory()->right(num); break;
	    default: assert(false);
	    }

	  switch (denomAlign)
	    {
	    case T_LEFT: break; // nothing to do
	    case T_CENTER: denom = ctxt.MGD()->getFactory()->center(denom); break;
	    case T_RIGHT: denom = ctxt.MGD()->getFactory()->right(denom); break;
	    default: assert(false);
	    }
	
	  res = ctxt.MGD()->fraction(ctxt, num, denom, thickness);
	}

      res = formatEmbellishment(this, ctxt, res);

      setArea(ctxt.MGD()->wrapper(ctxt, res));

      ctxt.pop();
      resetDirtyLayout();
    }

  return getArea();
}

void
MathMLFractionElement::setFlagDown(Flags f)
{
  MathMLContainerElement::setFlagDown(f);
  numerator.setFlagDown(f);
  denominator.setFlagDown(f);
}

Element *
MathMLFractionElement::getElementByFlag(Flags f)
{
    printf("[MathMLFractionElement::getElementByFlag]: numerator//\n");
    Element *__elem = numerator.getElementByFlag(f);
    if (__elem) {
        printf("[MathMLFractionElement::getElementByFlag]: found at numerator//\n");
        return __elem;
    }
    printf("[MathMLFractionElement::getElementByFlag]: denumerator//\n");
    __elem = denominator.getElementByFlag(f);
    if (__elem)
        return __elem;
    return nullptr;
}

void
MathMLFractionElement::resetFlagDown(Flags f)
{
  MathMLContainerElement::resetFlagDown(f);
  numerator.resetFlagDown(f);
  denominator.resetFlagDown(f);
}

SmartPtr<MathMLOperatorElement>
MathMLFractionElement::getCoreOperator()
{
  return getNumerator() ? getNumerator()->getCoreOperator() : nullptr;
}
