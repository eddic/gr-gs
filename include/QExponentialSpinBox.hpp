/*!
 * @file			QExponentialSpinBox.hpp
 * @brief		Defines an exponential QT spin box
 * @author		Eddie Carle &lt;eddie@isatec.ca&gt;
 * @date			March 6, 2015
 * @copyright	Copyright &copy; 2015 %Isatec Inc.  This project is released
 *					under the GNU General Public License Version 3.
 */

/* Copyright (C) 2015 %Isatec Inc.
 * 
 * This file is part of the %Isatec GNU Radio Module
 *
 * The %Isatec GNU Radio Module is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * The %Isatec GNU Radio Module is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *                                                                          
 * You should have received a copy of the GNU General Public License along with
 * The %Isatec GNU Radio Module.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QSpinBox>
#include <cmath>
#include <string>

//! GNU Radio Namespace
namespace gr
{
	//! Contains all blocks for the %Isatec GNU Radio Module
	namespace Isatec
	{
		//! Contains all custom QT widgets used in this project
		namespace Widgets
		{
			//! An exponential QT spinbox
			/*!
			 * This spinbox holds a positive integer value and displays that
			 * value as the result of a base raised to the power of said value. The
			 * base defaults to 2 but can be set with setBase().
			 *
			 * @date		March 6, 2015
			 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
			 */
			class QExponentialSpinBox: public QSpinBox
			{
			public:
				//! Sole constructor
				/*!
				 * Note that in order to improve efficiency of operation this
				 * function does not perform length/bounds checks on the input
				 * source word.  It is up to the calling function to ensure the
				 * parameters are correctly sized.
				 *
				 * @param	[in] parent Parent widget for destruction purposes.
				 * @param	[in] base Base of exponent.
				 * @date		March 6, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				QExponentialSpinBox(QWidget* parent=0, unsigned int base=2):
					QSpinBox(parent),
					m_base(base)
				{
					this->setMinimum(0);
				}

				//! Set exponent base
				/*!
				 * @param	[in] base Base of exponent.
				 * @date		March 6, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				void setBase(unsigned int base)
				{
					m_base = base;
				}

			protected:
				//! From QSpinBox
				QString textFromValue(int value) const
				{
					return QString::fromStdString(std::to_string(unsigned(std::pow(m_base,value))));
				}

				//! From QSpinBox
				/*!
				 * @return	The logarithm (using the base) of the integer represented
				 * 			by the passed string value. If the result is non-integer
				 * 			a -1 is returned.
				 * @date		March 6, 2015
				 * @author	Eddie Carle &lt;eddie@isatec.ca&gt;
				 */
				int valueFromText(const QString & text) const
				{
					const int desired = std::abs(std::stoi(text.toStdString()));
					if(desired>=1)
					{
						int value=0;
						int power;
						do
							power=std::pow(m_base,value++);
						while(power < desired);
						--value;

						if(power == desired)
							return value;
					}
					return -1;
				}

			private:
				unsigned int m_base; //!< Current exponent base
			};
		}
	}
}
