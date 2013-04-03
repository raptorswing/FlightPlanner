#include "WaysetTimelineWidget.h"

#include <QPainter>

WaysetTimelineWidget::WaysetTimelineWidget(QWidget *parent) :
    QWidget(parent)
{
}

const Wayset &WaysetTimelineWidget::currentWayset() const
{
    return _wayset;
}

const UAVParameters &WaysetTimelineWidget::uavParams() const
{
    return _uavParams;
}

//virtual from QWidget
QSize WaysetTimelineWidget::sizeHint() const
{
    return QSize(168,100);
}

//protected
//virtual from QWidget
void WaysetTimelineWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    //Draw background
    {
        painter.fillRect(this->rect(),
                         Qt::white);
    }

    //draw border
    {
        QRect borderRect = this->rect();

        const int penWidth = qMax<int>(1, painter.pen().width());
        borderRect.setHeight(borderRect.height() - penWidth);
        borderRect.setWidth(borderRect.width() - penWidth);
        painter.drawRect(borderRect);
    }

    //We don't do anything exciting unless we have at least two waypoints
    if (this->currentWayset().size() <= 1)
    {
        QFont font = painter.font();
        font.setPointSize(15);
        font.setLetterSpacing(QFont::PercentageSpacing, 150.0);
        painter.setFont(font);

        painter.setPen(this->palette().midlight().color());

        painter.drawText(this->rect(),
                         QString("---"),
                         QTextOption(Qt::AlignCenter));
        return;
    }

    const int dividingLineY = this->height() * 0.75;

    //Draw the waypoints along the timeline
    {
        const qreal flightLengthMeters = _wayset.lengthMeters(_uavParams);
        const qreal flightLengthSeconds = flightLengthMeters / _uavParams.airspeed();

        const QColor waypointColor = Qt::black;
        const QColor selectedWaypointColor = Qt::red;
        const int waypointWidth = 6;
        const int leftMargin = waypointWidth / 2;
        const int rightMargin = 20;

        const int availableSpace = this->width() - leftMargin - rightMargin;

        QList<QRect> textRects;
        const QList<qreal> waypointDistances = _wayset.waypointSubLengthsMeters(_uavParams);
        for (int i = 0; i < waypointDistances.size(); i++)
        {
            const qreal dist = waypointDistances.at(i);
            const qreal ratio = dist / flightLengthMeters;
            const int centerX = leftMargin + ratio * availableSpace;

            QColor color = waypointColor;
            if (_selectedIndices.contains(i))
                color = selectedWaypointColor;

            painter.fillRect(centerX - waypointWidth / 2, 0,
                             waypointWidth, dividingLineY,
                             color);

            //Calculate some text to put below the line
            const int waypointSeconds = flightLengthSeconds * ratio;
            const QString waypointSecondsText = QString::number(waypointSeconds) + "s";
            const int textWidth = painter.fontMetrics().width(waypointSecondsText);
            const QRect textRect(centerX - textWidth / 2 - 1, dividingLineY,
                                 textWidth + 2, this->height() - dividingLineY);

            //Draw the label text if we're not stepping on other text's toes
            bool drawText = true;
            foreach(const QRect& otherRect, textRects)
            {
                if (otherRect.intersects(textRect))
                    drawText = false;
            }
            if (drawText && i > 0)
            {
                painter.drawText(textRect,
                                 QString::number(waypointSeconds) + "s",
                                 QTextOption(Qt::AlignCenter));
                painter.drawLine(centerX, dividingLineY - 5,
                                 centerX, dividingLineY + 5);
                textRects.append(textRect);
            }
        }
    }

    //Draw a horizontal line
    {
        const QLine line(0, dividingLineY,
                         this->width(), dividingLineY);
        painter.drawLine(line);
    }
}

//public slot
void WaysetTimelineWidget::setWayset(const Wayset &wayset)
{
    _wayset = wayset;
    this->update();
}

//public slot
void WaysetTimelineWidget::setUAVParameters(const UAVParameters &uavParams)
{
    _uavParams = uavParams;
    this->update();
}

//public slot
void WaysetTimelineWidget::setSelectedIndices(const QSet<int> &indices)
{
    _selectedIndices = indices;
    this->update();
}
