/*
This file is part of Telegram Desktop,
the official desktop version of Telegram messaging app, see https://telegram.org

Telegram Desktop is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

It is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

In addition, as a special exception, the copyright holders give permission
to link the code of portions of this program with the OpenSSL library.

Full license: https://github.com/telegramdesktop/tdesktop/blob/master/LICENSE
Copyright (c) 2014-2016 John Preston, https://desktop.telegram.org
*/
#include "stdafx.h"
#include "window/section_widget.h"

#include "application.h"

namespace Window {

SectionWidget::SectionWidget(QWidget *parent) : TWidget(parent) {
}

void SectionWidget::setGeometryWithTopMoved(const QRect &newGeometry, int topDelta) {
	_topDelta = topDelta;
	bool willBeResized = (size() != newGeometry.size());
	if (geometry() != newGeometry) {
		setGeometry(newGeometry);
	}
	if (!willBeResized) {
		resizeEvent(nullptr);
	}
	_topDelta = 0;
}

void SectionWidget::showAnimated(SlideDirection direction, const SectionSlideParams &params) {
	if (_showAnimation) return;

	showChildren();
	auto myContentCache = grabForShowAnimation(params);
	hideChildren();
	showAnimatedHook();

	_showAnimation = std_::make_unique<SlideAnimation>();
	_showAnimation->setDirection(direction);
	_showAnimation->setRepaintCallback(func(this, &SectionWidget::repaintCallback));
	_showAnimation->setFinishedCallback(func(this, &SectionWidget::showFinished));
	_showAnimation->setPixmaps(params.oldContentCache, myContentCache);
	_showAnimation->setTopBarShadow(params.withTopBarShadow);
	_showAnimation->start();

	show();
}

void SectionWidget::paintEvent(QPaintEvent *e) {
	if (Ui::skipPaintEvent(this, e)) return;

	if (_showAnimation) {
		Painter p(this);
		_showAnimation->paintContents(p, e->rect());
	}
}

void SectionWidget::showFinished() {
	_showAnimation.reset();
	if (isHidden()) return;

	App::app()->mtpUnpause();

	showChildren();
	showFinishedHook();

	setInnerFocus();
}

} // namespace Window
