#include <QSpinBox>
#include <cmath>
#include <string>

class QExponentialSpinBox: public QSpinBox
{
public:
	QExponentialSpinBox(QWidget* parent=0):
		QSpinBox(parent),
		m_base(2)
	{
		this->setMinimum(1);
	}

	void setBase(unsigned int base)
	{
		m_base = base;
	}

protected:
	QString textFromValue(int value) const
	{
		return QString::fromStdString(std::to_string(unsigned(std::pow(m_base,value))));
	}

	int valueFromText(const QString & text) const
	{
		const unsigned int desired = std::abs(std::stoi(text.toStdString()));
		unsigned int value=0;
		unsigned int power;
		do
			power=std::pow(m_base,value++);
		while(power < desired);
		--value;

		if(power == desired)
			return value;
		else
			return 0;
	}

private:
	unsigned int m_base;
};
