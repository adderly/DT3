//==============================================================================
///	
///	File: EdLevelPropertyEnumField.cpp
///	
/// Copyright (C) 2000-2014 by Smells Like Donkey Software Inc. All rights reserved.
///
/// This file is subject to the terms and conditions defined in
/// file 'LICENSE.txt', which is part of this source code package.
///	
//==============================================================================

// Editor include
#include "EdLevelPropertyEnumField.hpp"
#include "EdLevelPropertiesWindow.hpp"
#include "EdLevelLineEdit.hpp"

// Qt include
#include <QtCore/QFile>
#include <QtWidgets/QLabel>
#include <QtWidgets/QGridLayout>

// Engine includes
#include "DT3Core/Types/FileBuffer/ArchiveData.hpp"
#include "DT3Core/Types/Utility/MoreStrings.hpp"
#include "DT3Core/Types/Node/PlugNode.hpp"

//==============================================================================
//==============================================================================

EdLevelPropertyEnumField::EdLevelPropertyEnumField (EdLevelPropertiesWindow *parent, std::shared_ptr<PlugNode> node, std::shared_ptr<ArchiveData> data)
{
    //setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    _data = data;
    _node = node;
   
    buildStatusButtons();

    QLabel *name = new QLabel(this);
    name->setMaximumWidth(130);
    name->setMinimumWidth(130);
    name->setText(MoreStrings::captialize_and_format(_data->title()).c_str());

    _value = new QComboBox(this);
    _value->setObjectName("prop");

    const std::vector<std::string> &enums = data->enums();
    for (DTuint i = 0; i < enums.size(); ++i) {
        _value->addItem(enums[i].c_str());
    }
    
	connect(	_value,         SIGNAL(activated(int)),
				this,           SLOT(doWriteParams())	);
    
	connect(	this,           SIGNAL(doCommand(QString, bool)),
				parent,         SLOT(onCommand(QString, bool))	);

    QGridLayout *layout = new QGridLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->setHorizontalSpacing(0);
    layout->setVerticalSpacing(0);
    layout->addWidget(keyButton(),0,0);
    layout->addWidget(hasInputButton(),0,1);
    layout->addWidget(hasOutputButton(),0,2);
    layout->addWidget(name,0,3);
    layout->addWidget(_value,0,4);

    setLayout(layout);
    
    setMinimumHeight(15+2);
    
    doReadParams();
}

EdLevelPropertyEnumField::~EdLevelPropertyEnumField	(void)
{

}

//==============================================================================
//==============================================================================

void EdLevelPropertyEnumField::doReadParams(void)
{
    blockSignals(true);

    keyButton()->disconnect();

    // Has Key
    if (_data->plug()) {
        keyButton()->setIcon(QIcon(":/images/key.png"));
                
        connect(    keyButton(),    SIGNAL(pressed()),
                    this,           SLOT(doKeyframePressed())    );
                    
    } else {
        keyButton()->setIcon(QIcon(":/images/blank.png"));
    }

    // Has Input
    if (_data->plug() && _data->plug()->has_incoming_connection()) {
        hasInputButton()->setIcon(QIcon(":/images/has_input.png"));
    } else {
        hasInputButton()->setIcon(QIcon(":/images/blank.png"));
    }

    // Has Output
    if (_data->plug() && _data->plug()->has_outgoing_connection()) {
        hasOutputButton()->setIcon(QIcon(":/images/has_output.png"));
    } else {
        hasOutputButton()->setIcon(QIcon(":/images/blank.png"));
    }

	TextBufferStream stream;
	_data->value(stream);
    _value->setCurrentIndex( MoreStrings::cast_from_string<int>(stream.buffer()) );
    blockSignals(false);
}

void EdLevelPropertyEnumField::doWriteParams(void)
{
    std::string index = MoreStrings::cast_to_string(_value->currentIndex());

    TextBufferStream oldstream;
	_data->value(oldstream);
    
    // Only if value changed
    if (index != oldstream.buffer()) {
        emit doCommand(QString("SetProp \"") + _node->full_name().c_str() + "." + _data->title().c_str() + "\" \"" + index.c_str() + "\"", _data->flags() & DATA_FLUSH_UI);
    }
}

//==============================================================================
//==============================================================================

void EdLevelPropertyEnumField::doKeyframePressed (void)
{
    emit doKeyMenu( QString(_node->full_name().c_str()) + "." + _data->title().c_str(), keyButton()->mapToGlobal(QPoint(0,0)));
}

//==============================================================================
//==============================================================================

std::string EdLevelPropertyEnumField::getValueOfField (void)
{
	TextBufferStream stream;
	_data->value(stream);
    return stream.buffer();
}
    
void EdLevelPropertyEnumField::setValueOfField (const std::string &value)
{
	TextBufferStream stream(value);
	_data->set_value(stream);
    doReadParams();

    std::string index = MoreStrings::cast_to_string(_value->currentIndex());
    emit doCommand(QString("SetProp \"") + _node->full_name().c_str() + "." + _data->title().c_str() + "\" \"" + index.c_str() + "\"", _data->flags() & DATA_FLUSH_UI);
}

//==============================================================================
//==============================================================================

#include "moc_EdLevelPropertyEnumField.cpp"

