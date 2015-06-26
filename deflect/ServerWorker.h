/*********************************************************************/
/* Copyright (c) 2011 - 2012, The University of Texas at Austin.     */
/* All rights reserved.                                              */
/*                                                                   */
/* Redistribution and use in source and binary forms, with or        */
/* without modification, are permitted provided that the following   */
/* conditions are met:                                               */
/*                                                                   */
/*   1. Redistributions of source code must retain the above         */
/*      copyright notice, this list of conditions and the following  */
/*      disclaimer.                                                  */
/*                                                                   */
/*   2. Redistributions in binary form must reproduce the above      */
/*      copyright notice, this list of conditions and the following  */
/*      disclaimer in the documentation and/or other materials       */
/*      provided with the distribution.                              */
/*                                                                   */
/*    THIS  SOFTWARE IS PROVIDED  BY THE  UNIVERSITY OF  TEXAS AT    */
/*    AUSTIN  ``AS IS''  AND ANY  EXPRESS OR  IMPLIED WARRANTIES,    */
/*    INCLUDING, BUT  NOT LIMITED  TO, THE IMPLIED  WARRANTIES OF    */
/*    MERCHANTABILITY  AND FITNESS FOR  A PARTICULAR  PURPOSE ARE    */
/*    DISCLAIMED.  IN  NO EVENT SHALL THE UNIVERSITY  OF TEXAS AT    */
/*    AUSTIN OR CONTRIBUTORS BE  LIABLE FOR ANY DIRECT, INDIRECT,    */
/*    INCIDENTAL,  SPECIAL, EXEMPLARY,  OR  CONSEQUENTIAL DAMAGES    */
/*    (INCLUDING, BUT  NOT LIMITED TO,  PROCUREMENT OF SUBSTITUTE    */
/*    GOODS  OR  SERVICES; LOSS  OF  USE,  DATA,  OR PROFITS;  OR    */
/*    BUSINESS INTERRUPTION) HOWEVER CAUSED  AND ON ANY THEORY OF    */
/*    LIABILITY, WHETHER  IN CONTRACT, STRICT  LIABILITY, OR TORT    */
/*    (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY WAY OUT    */
/*    OF  THE  USE OF  THIS  SOFTWARE,  EVEN  IF ADVISED  OF  THE    */
/*    POSSIBILITY OF SUCH DAMAGE.                                    */
/*                                                                   */
/* The views and conclusions contained in the software and           */
/* documentation are those of the authors and should not be          */
/* interpreted as representing official policies, either expressed   */
/* or implied, of The University of Texas at Austin.                 */
/*********************************************************************/

#ifndef DEFLECT_SERVER_WORKER_H
#define DEFLECT_SERVER_WORKER_H

#include <deflect/Event.h>
#include <deflect/EventReceiver.h>
#include <deflect/MessageHeader.h>
#include <deflect/Segment.h>

#include <QtNetwork/QTcpSocket>
#include <QQueue>

namespace deflect
{

class ServerWorker : public EventReceiver
{
    Q_OBJECT

public:
    ServerWorker( int socketDescriptor );
    ~ServerWorker();

public slots:
    void processEvent( Event evt ) final;

    void initConnection();
    void closeConnection( QString uri );
    void replyToEventRegistration( QString uri, bool success );

signals:
    void receivedAddPixelStreamSource( QString uri, size_t sourceIndex );
    void receivedPixelStreamSegement( QString uri, size_t sourceIndex,
                                      deflect::Segment segment );
    void receivedPixelStreamFinishFrame( QString uri, size_t sourceIndex );
    void receivedRemovePixelStreamSource( QString uri, size_t sourceIndex );

    void registerToEvents( QString uri, bool exclusive,
                           deflect::EventReceiver* receiver);

    void receivedCommand( QString command, QString senderUri );

    void connectionClosed();

    /** @internal */
    void _dataAvailable();

private slots:
    void _processMessages();

private:
    int _socketDescriptor;
    QTcpSocket* _tcpSocket;

    QString _streamUri;

    bool _registeredToEvents;
    QQueue<Event> _events;

    void _receiveMessage();
    MessageHeader _receiveMessageHeader();
    QByteArray _receiveMessageBody( int size );

    void _handleMessage( const MessageHeader& messageHeader,
                         const QByteArray& byteArray );
    void _handlePixelStreamMessage( const QString& uri,
                                    const QByteArray& byteArray );

    void _sendProtocolVersion();
    void _sendBindReply( bool successful );
    void _send( const Event &evt );
    void _sendQuit();
    bool _send( const MessageHeader& messageHeader );
    void _flushSocket();
};

}

#endif
