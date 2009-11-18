// -*-c++-*-

/***************************************************************************
                                resultsaver.cpp
                    Abstract Base Class for saving results
                             -------------------
    begin                : 06-JUNE-2004
    copyright            : (C) 2004 by The RoboCup Soccer Simulator
                           Maintenance Group.
    email                : sserver-admin@lists.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LGPL as published by the Free Software  *
 *   Foundation; either version 3 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "resultsaver.hpp"

ResultSaver::FactoryHolder &
ResultSaver::factory()
{
    static FactoryHolder fact;
    return fact;
}
