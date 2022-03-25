// -*-c++-*-

/***************************************************************************
                                dispsender.h
                     Classes for building display messages
                             -------------------
    begin                : 2007-11-22
    copyright            : (C) 2007 by The RoboCup Soccer Server
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

#ifndef RCSS_DISP_SENDER_H
#define RCSS_DISP_SENDER_H

#include "sender.h"
#include "observer.h"
#include "types.h"

#include <rcss/factory.hpp>

#include <memory>

class Stadium;
class Logger;
class Monitor;

namespace rcss {

class SerializerMonitor;

/*!
  \class DispSender
  \brief Base class for the display protocol.
*/
class DispSender
    : protected Sender {
protected:

    DispSender( std::ostream & transport );

public:

    virtual
    ~DispSender() override;

    virtual
    void sendShow() = 0;

    virtual
    void sendMsg( const BoardType board,
                  const char * msg ) = 0;

    virtual
    void sendTeamGraphic( const Side side,
                          const unsigned int x,
                          const unsigned int y ) = 0;
};

//===================================================================
//===================================================================
//===================================================================

/*!
  \class DispSenderMonitor
  \brief Base class for the visual protocol for monitors.
*/
class DispSenderMonitor
    : public DispSender {
public:

    class Params {
    public:
        std::ostream & M_transport;
        const Monitor & M_self;
        const std::shared_ptr< SerializerMonitor > M_serializer;
        const Stadium & M_stadium;

        Params( std::ostream & transport,
                const Monitor & self,
                const std::shared_ptr< SerializerMonitor > serializer,
                const Stadium & stadium )
            : M_transport( transport )
            , M_self( self )
            , M_serializer( serializer )
            , M_stadium( stadium )
          { }
    };


private:
    const std::shared_ptr< SerializerMonitor > M_serializer;

    /*:TODO: M_self needs to be replaced with a reference to a
      Observer and Observer should have virtual functions for
      stuff like velocity, stamina, etc */
    const Monitor & M_self;
    const Stadium & M_stadium;

public:
    typedef std::shared_ptr< DispSenderMonitor > Ptr;
    typedef Ptr (*Creator)( const DispSenderMonitor::Params & );
    typedef rcss::Factory< Creator, int > FactoryHolder;

    static
    FactoryHolder & factory();

protected:
    DispSenderMonitor( const Params & params );

public:
    virtual
    ~DispSenderMonitor() override;

protected:

    const
    SerializerMonitor & serializer() const
      {
          return *M_serializer;
      }

    const
    Monitor & self() const
      {
          return M_self;
      }

    const
    Stadium & stadium() const
      {
          return M_stadium;
      }
};


/*!
  \class ObserverMonitor
  \brief Interface for monitors to send display information.
*/
class ObserverMonitor
    : protected BaseObserver< DispSenderMonitor > {
public:

    ObserverMonitor()
      { }

    ObserverMonitor( std::shared_ptr< DispSenderMonitor > sender )
        : BaseObserver< DispSenderMonitor >( sender )
      { }

    ~ObserverMonitor()
      { }

    void setDispSender( std::shared_ptr< DispSenderMonitor > sender )
      {
          BaseObserver< DispSenderMonitor >::setSender( sender );
      }

    void sendShow()
      {
          BaseObserver< DispSenderMonitor >::sender().sendShow();
      }

    void sendMsg( const BoardType board,
                  const char * msg )
      {
          BaseObserver< DispSenderMonitor >::sender().sendMsg( board, msg );
      }

    void sendTeamGraphic( const Side side,
                          const unsigned int x,
                          const unsigned int y )
      {
          BaseObserver< DispSenderMonitor >::sender().sendTeamGraphic( side, x, y );
      }
};


/*!
  \class DispSenderMonitorV1
  \brief class for the version 1 display protocol.
*/
class DispSenderMonitorV1
    : public DispSenderMonitor {
public:

    DispSenderMonitorV1( const Params & params );

    virtual
    ~DispSenderMonitorV1() override;

    virtual
    void sendShow() override;

    virtual
    void sendMsg( const BoardType board,
                  const char * msg ) override;

    virtual
    void sendTeamGraphic( const Side side,
                          const unsigned int x,
                          const unsigned int y ) override;
};


/*!
  \class DispSenderMonitorV2
  \brief class for the version 2 display protocol.
*/
class DispSenderMonitorV2
    : public DispSenderMonitorV1 {
public:

    DispSenderMonitorV2( const Params & params );

    virtual
    ~DispSenderMonitorV2() override;

    virtual
    void sendShow() override;

    virtual
    void sendMsg( const BoardType board,
                  const char * msg ) override;

    //virtual
    //void sendTeamGraphic( const Side side,
    //                      const unsigned int x,
    //                      const unsigned int y );
};

/*!
  \class DispSenderMonitorV3
  \brief class for the version 3 display protocol.
*/
class DispSenderMonitorV3
    : public DispSenderMonitorV2 {
public:

    DispSenderMonitorV3( const Params & params );

    virtual
    ~DispSenderMonitorV3() override;

    virtual
    void sendShow() override;

    virtual
    void sendMsg( const BoardType board,
                  const char * msg ) override;

    //virtual
    //void sendTeamGraphic( const Side side,
    //                      const unsigned int x,
    //                      const unsigned int y );
};


// /*!
//   \class DispSenderMonitorV4
//   \brief class for the version 4 display protocol.
// */
// class DispSenderMonitorV4
//     : public DispSenderMonitorV3 {
// public:

//     DispSenderMonitorV4( const Params & params );

//     virtual
//     ~DispSenderMonitorV4();

//     virtual
//     void sendShow();

//     virtual
//     void sendMsg( const BoardType board,
//                   const char * msg );

//     virtual
//     void sendTeamGraphic( const Side side,
//                           const unsigned int x,
//                           const unsigned int y );
// };

/*!
  \class DispSenderMonitorJSON
  \brief class for the version 5 monitor protocol. (JSON format)
*/
class DispSenderMonitorJSON
    : public DispSenderMonitor {
public:

    DispSenderMonitorJSON( const Params & params );

    virtual
    ~DispSenderMonitorJSON() override;

    virtual
    void sendShow() override;

    virtual
    void sendMsg( const BoardType board,
                  const char * msg ) override;

    virtual
    void sendTeamGraphic( const Side side,
                         const unsigned int x,
                         const unsigned int y ) override;
};




//===================================================================
//===================================================================
//===================================================================

class DispSenderLogger
    : public DispSender {
public:

    class Params {
    public:
        std::ostream & M_transport;
        const Logger & M_self;
        const std::shared_ptr< SerializerMonitor > M_serializer;
        const Stadium & M_stadium;

        Params( std::ostream & transport,
                const Logger & self,
                const std::shared_ptr< SerializerMonitor > serializer,
                const Stadium & stadium )
            : M_transport( transport ),
              M_self( self ),
              M_serializer( serializer ),
              M_stadium( stadium )
          { }
    };

private:
    const std::shared_ptr< SerializerMonitor > M_serializer;

    /*:TODO: M_self needs to be replaced with a reference to a
      Observer and Observer should have virtual functions for
      stuff like velocity, stamina, etc */
    const Logger & M_self;
    const Stadium & M_stadium;

public:
    typedef std::shared_ptr< DispSenderLogger > Ptr;
    typedef Ptr (*Creator)( const DispSenderLogger::Params & );
    typedef rcss::Factory< Creator, int > FactoryHolder;

    static
    FactoryHolder & factory();

protected:
    DispSenderLogger( const Params & params );

public:
    ~DispSenderLogger() override;

protected:

    const
    SerializerMonitor & serializer() const
      {
          return *M_serializer;
      }

    const
    Logger & self() const
      {
          return M_self;
      }

    const
    Stadium & stadium() const
      {
          return M_stadium;
      }

};

/*!
  \class ObserverLogger
  \brief Interface for logger to recorde the display information.
*/
class ObserverLogger
    : protected BaseObserver< DispSenderLogger > {
public:

    ObserverLogger()
      { }

    ObserverLogger( std::shared_ptr< DispSenderLogger > sender )
        : BaseObserver< DispSenderLogger >( sender )
      { }

    ~ObserverLogger()
      { }

    void setDispSender( std::shared_ptr< DispSenderLogger > sender )
      {
          BaseObserver< DispSenderLogger >::setSender( sender );
      }

    void sendShow()
      {
          BaseObserver< DispSenderLogger >::sender().sendShow();
      }

    void sendMsg( const BoardType board,
                  const char * msg )
      {
          BaseObserver< DispSenderLogger >::sender().sendMsg( board, msg );
      }

    void sendTeamGraphic( const Side side,
                          const unsigned int x,
                          const unsigned int y )
      {
          BaseObserver< DispSenderLogger >::sender().sendTeamGraphic( side, x, y );
      }
};


/*!
  \class DispSenderLoggerV1
  \brief class for the log version 1
 */
class DispSenderLoggerV1
    : public DispSenderLogger {
public:

    DispSenderLoggerV1( const Params & params );

    virtual
    ~DispSenderLoggerV1() override;

    virtual
    void sendShow() override;

    virtual
    void sendMsg( const BoardType board,
                  const char * msg ) override;

    virtual
    void sendTeamGraphic( const Side side,
                          const unsigned int x,
                          const unsigned int y ) override;

};


/*!
  \class DispSenderLoggerV2
  \brief class for the log version 2
 */
class DispSenderLoggerV2
    : public DispSenderLoggerV1 {
public:

    DispSenderLoggerV2( const Params & params );

    virtual
    ~DispSenderLoggerV2() override;

    virtual
    void sendShow() override;

    virtual
    void sendMsg( const BoardType board,
                  const char * msg ) override;

};


/*!
  \class DispSenderLoggerV3
  \brief class for the log version 3
 */
class DispSenderLoggerV3
    : public DispSenderLoggerV2 {
public:

    DispSenderLoggerV3( const Params & params );

    virtual
    ~DispSenderLoggerV3() override;

    virtual
    void sendShow() override;

    //virtual
    //void sendMsg( const BoardType board,
    //              const char * msg );

};

/*!
  \class DispSenderLoggerV4
  \brief class for the log version 4
 */
class DispSenderLoggerV4
    : public DispSenderLoggerV3 {
public:

    DispSenderLoggerV4( const Params & params );

    virtual
    ~DispSenderLoggerV4() override;

    virtual
    void sendShow() override;

    virtual
    void sendMsg( const BoardType board,
                  const char * msg ) override;

};

/*!
  \class DispSenderLoggerV6
  \brief class for the log version 6 (JSON format)
 */
class DispSenderLoggerJSON
    : public DispSenderLogger {
public:

    DispSenderLoggerJSON( const Params & params );

    virtual
    ~DispSenderLoggerJSON() override;

    virtual
    void sendShow() override;

    virtual
    void sendMsg( const BoardType board,
                  const char * msg );

    virtual
    void sendTeamGraphic( const Side side,
                          const unsigned int x,
                          const unsigned int y ) override;
};


}

#endif
