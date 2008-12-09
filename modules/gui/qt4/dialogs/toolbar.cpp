/*****************************************************************************
 * ToolbarEdit.cpp : ToolbarEdit and About dialogs
 ****************************************************************************
 * Copyright (C) 2008 the VideoLAN team
 * $Id$
 *
 * Authors: Jean-Baptiste Kempf <jb (at) videolan.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "dialogs/toolbar.hpp"

#include "util/input_slider.hpp"
#include "util/customwidgets.hpp"
#include "components/interface_widgets.hpp"

#include <QScrollArea>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QListWidget>

#include <QDragEnterEvent>
#include <QDialogButtonBox>

ToolbarEditDialog *ToolbarEditDialog::instance = NULL;

ToolbarEditDialog::ToolbarEditDialog( intf_thread_t *_p_intf)
                  : QVLCFrame(  _p_intf )
{
    setWindowTitle( qtr( "Toolbars Editor" ) );
    QGridLayout *mainLayout = new QGridLayout( this );
    setMinimumWidth( 600 );

    /* main GroupBox */
    QGroupBox *widgetBox = new QGroupBox( "Toolbar Elements", this );
    widgetBox->setSizePolicy( QSizePolicy::Preferred,
                              QSizePolicy::MinimumExpanding );
    QGridLayout *boxLayout = new QGridLayout( widgetBox );

    flatBox = new QCheckBox( qtr( "Flat Button" ) );
    bigBox = new QCheckBox( qtr( "Big Button" ) );
    shinyBox = new QCheckBox( qtr( "Native Slider" ) );
    shinyBox->setChecked( true );

    boxLayout->addWidget( new WidgetListing( p_intf, this ), 0, 0, 1, -1);
    boxLayout->addWidget( flatBox, 1, 0 );
    boxLayout->addWidget( bigBox, 1, 1 );
    boxLayout->addWidget( shinyBox, 1, 2 );
    mainLayout->addWidget( widgetBox, 0, 0, 1, -1 );


    /* Main ToolBar */
    QGroupBox *mainToolbarBox = new QGroupBox( "Main Toolbar", this );
    QGridLayout *mainTboxLayout = new QGridLayout( mainToolbarBox );

    QLabel *label = new QLabel( "Toolbar position:" );
    mainTboxLayout->addWidget(label, 0, 0, 1, 2);

    QComboBox *positionCombo = new QComboBox;
    positionCombo->addItems( QStringList() << "Above the Video"
                                           << "Under the Video" );
    mainTboxLayout->addWidget( positionCombo, 0, 2, 1, 1 );

    QLabel *line1Label = new QLabel( "Line 1:" );
    QString line1 = getSettings()->value( "MainWindow/Controls1",
                        "64;36;37;38;65").toString();
    controller1 = new DroppingController( p_intf, line1,
            this );
    mainTboxLayout->addWidget( line1Label, 1, 0, 1, 1 );
    mainTboxLayout->addWidget( controller1, 1, 1, 1, 2 );

    QLabel *line2Label = new QLabel( "Line 2:" );
    QString line2 = getSettings()->value( "MainWindow/Controls2",
                        "0-2;64;3;1;4;64;7;10;9;65;34-4;" ).toString();
    controller2 = new DroppingController( p_intf, line2,
            this );
    mainTboxLayout->addWidget( line2Label, 2, 0, 1, 1 );
    mainTboxLayout->addWidget( controller2, 2, 1, 1, 2);

    /* Advanced ToolBar */
    QLabel *advLabel = new QLabel( "Advanced Widget toolbar:" );
    QString lineA = getSettings()->value( "MainWindow/AdvControl",
                        "12;11;13;14").toString();
    controllerA = new DroppingController( p_intf, lineA,
            this );
    mainTboxLayout->addWidget( advLabel, 3, 0, 1, 2 );
    mainTboxLayout->addWidget( controllerA, 3, 2, 1, 1 );

    mainLayout->addWidget( mainToolbarBox, 1, 0, 1, -1 );

    /* TimeToolBar */
    QGroupBox *timeToolbarBox = new QGroupBox( "Time Toolbar", this );
    QGridLayout *timeTboxLayout = new QGridLayout( timeToolbarBox );

    QString line = getSettings()->value( "MainWindow/InputControl",
                        "5-1;33;6-1").toString();
    controller = new DroppingController( p_intf, line,
            this );
    timeTboxLayout->addWidget( controller, 0, 0, 1, -1 );

    mainLayout->addWidget( timeToolbarBox, 2, 0, 1, -1 );

    /* FSCToolBar */
    QGroupBox *FSCToolbarBox = new QGroupBox( "Fullscreen Controller", this );
    QGridLayout *FSCTboxLayout = new QGridLayout( FSCToolbarBox );

    QString lineFSC = getSettings()->value( "MainWindow/FSCline",
                       "0-2;64;3;1;4;64;36;64;37;64;8;65;35-4;34" ).toString();
    controllerFSC = new DroppingController( p_intf,
            lineFSC, this );
    FSCTboxLayout->addWidget( controllerFSC, 0, 0, 1, -1 );

    mainLayout->addWidget( FSCToolbarBox, 3, 0, 1, -1 );

    /* Buttons */
    QDialogButtonBox *okCancel = new QDialogButtonBox;
    QPushButton *okButton = new QPushButton( qtr( "Cl&ose" ), this );
    QPushButton *cancelButton = new QPushButton( qtr( "&Cancel" ), this );
    okCancel->addButton( okButton, QDialogButtonBox::AcceptRole );
    okCancel->addButton( cancelButton, QDialogButtonBox::RejectRole );

    BUTTONACT( okButton, close() );
    BUTTONACT( cancelButton, cancel() );
    mainLayout->addWidget( okCancel, 4, 2 );
}


ToolbarEditDialog::~ToolbarEditDialog()
{
}

void ToolbarEditDialog::close()
{
    msg_Dbg( p_intf, "Close and save" );
    hide();
    getSettings()->setValue( "MainWindow/Controls1", controller1->getValue() );
    getSettings()->setValue( "MainWindow/Controls2", controller2->getValue() );
    getSettings()->setValue( "MainWindow/AdcControl", controllerA->getValue() );
    getSettings()->setValue( "MainWindow/InputControl", controller->getValue() );
    getSettings()->setValue( "MainWindow/FSCline", controllerFSC->getValue() );
}

void ToolbarEditDialog::cancel()
{
    hide();
}

WidgetListing::WidgetListing( intf_thread_t *p_intf, QWidget *_parent )
              : QListWidget( _parent )
{
    /* We need the parent to know the options checked */
    parent = qobject_cast<ToolbarEditDialog *>(_parent);
    assert( parent );

    /* Normal options */
    setViewMode( QListView::IconMode );
    setSpacing( 20 );
    setDragEnabled( true );
    setMinimumHeight( 250 );

    /* All the buttons do not need a special rendering */
    for( int i = 0; i < BUTTON_MAX; i++ )
    {
        QListWidgetItem *widgetItem = new QListWidgetItem( this );
        widgetItem->setText( nameL[i] );
        widgetItem->setIcon( QIcon( iconL[i] ) );
        widgetItem->setData( Qt::UserRole, QVariant( i ) );
        addItem( widgetItem );
    }

    /* Spacers are yet again a different thing */
    QListWidgetItem *widgetItem = new QListWidgetItem( QIcon( ":/space" ),
            qtr( "Spacer" ), this );
    widgetItem->setData( Qt::UserRole, WIDGET_SPACER );
    addItem( widgetItem );

    widgetItem = new QListWidgetItem( QIcon( ":/space" ),
            qtr( "Expanding Spacer" ), this );
    widgetItem->setData( Qt::UserRole, WIDGET_SPACER_EXTEND );
    addItem( widgetItem );

    /**
     * For all other widgets, we create then, do a pseudo rendering in
     * a pixmaps for the view, and delete the object
     *
     * A lot of code is retaken from the Abstract, but not exactly...
     * So, rewrite.
     * They are better ways to deal with this, but I doubt that this is
     * necessary. If you feel like you have the time, be my guest.
     * --
     * jb
     **/
    for( int i = SPLITTER; i < SPECIAL_MAX; i++ )
    {
        QWidget *widget = NULL;
        QListWidgetItem *widgetItem = new QListWidgetItem( this );
        switch( i )
        {
        case SPLITTER:
            {
                QFrame *line = new QFrame( this );
                line->setFrameShape( QFrame::VLine );
                line->setFrameShadow( QFrame::Raised );
                line->setLineWidth( 0 ); line->setMidLineWidth( 1 );
                widget = line;
            }
            widgetItem->setText( qtr("Splitter") );
            break;
        case INPUT_SLIDER:
            {
                InputSlider *slider = new InputSlider( Qt::Horizontal, this );
                widget = slider;
            }
            widgetItem->setText( qtr("Time Slider") );
            break;
        case VOLUME:
            {
                SoundWidget *snd = new SoundWidget( this, p_intf,
                        parent->getOptions() & WIDGET_SHINY );
                widget = snd;
            }
            widgetItem->setText( qtr("Volume") );
            break;
        case TIME_LABEL:
            {
                QLabel *timeLabel = new QLabel( "12:42/2:12:42", this );
                widget = timeLabel;
            }
            widgetItem->setText( qtr("Time") );
            break;
        case MENU_BUTTONS:
            {
                QWidget *discFrame = new QWidget( this );
                QHBoxLayout *discLayout = new QHBoxLayout( discFrame );
                discLayout->setSpacing( 0 ); discLayout->setMargin( 0 );

                QToolButton *prevSectionButton = new QToolButton( discFrame );
                prevSectionButton->setIcon( QIcon( ":/dvd_prev" ) );
                discLayout->addWidget( prevSectionButton );

                QToolButton *menuButton = new QToolButton( discFrame );
                menuButton->setIcon( QIcon( ":/dvd_menu" ) );
                discLayout->addWidget( menuButton );

                QToolButton *nextButton = new QToolButton( discFrame );
                nextButton->setIcon( QIcon( ":/dvd_next" ) );
                discLayout->addWidget( nextButton );

                widget = discFrame;
            }
            widgetItem->setText( qtr("DVD menus") );
            break;
        case TELETEXT_BUTTONS:
            {
                QWidget *telexFrame = new QWidget( this );
                QHBoxLayout *telexLayout = new QHBoxLayout( telexFrame );
                telexLayout->setSpacing( 0 ); telexLayout->setMargin( 0 );

                QToolButton *telexOn = new QToolButton( telexFrame );
                telexOn->setIcon( QIcon( ":/tv" ) );
                telexLayout->addWidget( telexOn );

                QToolButton *telexTransparent = new QToolButton;
                telexOn->setIcon( QIcon( ":/tvtelx-trans" ) );
                telexLayout->addWidget( telexTransparent );

                QSpinBox *telexPage = new QSpinBox;
                telexLayout->addWidget( telexPage );

                widget = telexFrame;
            }
            widgetItem->setText( qtr("Teletext") );
            break;
        case ADVANCED_CONTROLLER:
            {
                AdvControlsWidget *advControls = new AdvControlsWidget( p_intf, this );
                widget = advControls;
            }
            widgetItem->setText( qtr("Advanced Buttons") );
            break;
        default:
            msg_Warn( p_intf, "This should not happen %i", i );
            break;
        }

        if( widget == NULL ) continue;


        widgetItem->setIcon( QIcon( QPixmap::grabWidget( widget ) ) );
        widget->hide();
        widgetItem->setData( Qt::UserRole, QVariant( i ) );

        addItem( widgetItem );
        delete widget;
    }
}

void WidgetListing::startDrag( Qt::DropActions /*supportedActions*/ )
{
    QListWidgetItem *item =currentItem();

    QByteArray itemData;
    QDataStream dataStream( &itemData, QIODevice::WriteOnly );

    int i_type = item->data( Qt::UserRole ).toInt();
    int i_option = parent->getOptions();
    dataStream << i_type << i_option;

    QMimeData *mimeData = new QMimeData;
    mimeData->setData( "vlc/button-bar", itemData );

    QDrag *drag = new QDrag( this );
    drag->setMimeData( mimeData );
//    drag->setHotSpot(QPoint(pixmap.width()/2, pixmap.height()/2));

    drag->exec(Qt::CopyAction | Qt::MoveAction );
}


DroppingController::DroppingController( intf_thread_t *_p_intf, QString line, QWidget *_parent )
                   : AbstractController( _p_intf, _parent )
{
    rubberband = NULL;
    setAcceptDrops( true );
    controlLayout = new QHBoxLayout( this );
    controlLayout->setSpacing( 0 );
    controlLayout->setMargin( 0 );
    setFrameShape( QFrame::StyledPanel );
    setFrameShadow( QFrame::Raised );


    parseAndCreate( line, controlLayout );

}

/* Overloading the AbstractController one, because we don't manage the
   Spacing in the same ways */
void DroppingController::createAndAddWidget( QBoxLayout *controlLayout,
                                             int i_index,
                                             buttonType_e i_type,
                                             int i_option )
{
    doubleInt *value = new doubleInt;
    value->i_type = i_type;
    value->i_option = i_option;

    /* Special case for SPACERS, who aren't QWidgets */
    if( i_type == WIDGET_SPACER || i_type == WIDGET_SPACER_EXTEND )
    {
        QLabel *label = new QLabel;
        label->setPixmap( QPixmap( ":/space" ) );
        if( i_type == WIDGET_SPACER_EXTEND )
        {
            label->setScaledContents( true );
            label->setSizePolicy( QSizePolicy::MinimumExpanding,
                    QSizePolicy::Preferred );
        }
        else
            label->setSizePolicy( QSizePolicy::Fixed,
                    QSizePolicy::Preferred );

        controlLayout->insertWidget( i_index, label );
    }
    else
    {
        QWidget *widg = createWidget( i_type, i_option );
        if( !widg ) return;

        /* Some Widgets are deactivated at creation */
        widg->setEnabled( true );
        widg->show();
        controlLayout->insertWidget( i_index, widg );
    }
    /* QList and QBoxLayout don't act the same with insert() */
    if( i_index < 0 ) i_index = controlLayout->count() -1 ;

    widgetList.insert( i_index, value );
}

QString DroppingController::getValue()
{
    QString qs = "";

    for( int i = 0; i < controlLayout->count(); i++ )
    {
        doubleInt *dI = widgetList.at( i );
        assert( dI );

        qs.append( QString::number( dI->i_type ) );
        if( dI->i_option ) qs.append( "-" + QString::number( dI->i_option ) );
        qs.append( ';' );
    }
    return qs;
}

void DroppingController::dragEnterEvent( QDragEnterEvent * event )
{
    if( event->mimeData()->hasFormat( "vlc/button-bar" ) )
        event->accept();
    else
        event->ignore();
}

void DroppingController::dragMoveEvent( QDragMoveEvent *event )
{
    QPoint origin = event->pos();

    int i_pos = getParentPosInLayout( origin );
    bool b_end = false;

    /* Both sides of the frame */
    if( i_pos == -1 )
    {
        if( rubberband ) rubberband->hide();
        return;
    }

    /* Last item is special because of underlying items */
    if( i_pos >= controlLayout->count() )
    {
        i_pos--;
        b_end = true;
    }

    /* Query the underlying item for size && middles */
    QLayoutItem *tempItem = controlLayout->itemAt( i_pos ); assert( tempItem );
    QWidget *temp = tempItem->widget(); assert( temp );

    /* Position assignment */
    origin.ry() = 0;
    origin.rx() = temp->x() - 2;

    if( b_end ) origin.rx() += temp->width();

    if( !rubberband )
        rubberband = new QRubberBand( QRubberBand::Line, this );
    rubberband->setGeometry( origin.x(), origin.y(), 4, height() );
    rubberband->show();
}

inline int DroppingController::getParentPosInLayout( QPoint point)
{
    point.ry() = height() / 2 ;
    QPoint origin = mapToGlobal ( point );

    QWidget *tempWidg = QApplication::widgetAt( origin );

    int i = -1;
    if( tempWidg != NULL)
    {
        i = controlLayout->indexOf( tempWidg );
        if( i == -1 )
        {
            i = controlLayout->indexOf( tempWidg->parentWidget() );
            tempWidg = tempWidg->parentWidget();
        }
    }

    /* Return the nearest position */
    if( ( point.x() - tempWidg->x()  > tempWidg->width() / 2 ) && i != -1 )
        i++;

    //    msg_Dbg( p_intf, "%i", i);
    return i;
}

void DroppingController::dropEvent( QDropEvent *event )
{
    int i = getParentPosInLayout( event->pos() );

    QByteArray data = event->mimeData()->data( "vlc/button-bar" );
    QDataStream dataStream(&data, QIODevice::ReadOnly);

    int i_option = 0, i_type = 0;
    dataStream >> i_type >> i_option;

    createAndAddWidget( controlLayout, i, (buttonType_e)i_type, i_option );

    /* Hide by precaution, you don't exactly know what could have happened in
       between */
    if( rubberband ) rubberband->hide();
}

void DroppingController::dragLeaveEvent ( QDragLeaveEvent * event )
{
    if( rubberband ) rubberband->hide();
}

/**
 * Overloading doAction to block any action
 **/
void DroppingController::doAction( int i ){}

