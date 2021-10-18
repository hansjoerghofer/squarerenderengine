#include "GUI/LogWidget.h"

const std::string& LogWidget::name() const
{
    return m_title;
}

bool LogWidget::visible() const
{
	return m_visible;
}

void LogWidget::setVisible(bool flag)
{
	m_visible = flag;
}
