#pragma once

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <set>
#include <string>
#include "utils/Connection.hpp"
#include "blackboard/Blackboard.hpp"
#include "blackboard/Adapter.hpp"
#include "transmitter/TransmitterDefs.hpp"

/**
 * Adapter that allows Vision to communicate with the Blackboard
 * heavily mimics the boost chat server example at
 * http://www.boost.org/doc/libs/1_40_0/doc/html/boost_asio/examples.html
 * except without a message queue
 */
class OffNaoTransmitter : Adapter {
   public:
      /**
       * Constructor.  Opens a socket for listening.
       */
      OffNaoTransmitter(Blackboard *bb);
      /**
       * Destructor. Closes the socket.
       */
      ~OffNaoTransmitter();
      /**
       * One cycle of this thread
       */
      void tick();

   private:
      /**
       * the io_service to poll each tick
       */
      boost::asio::io_service io_service_;

      /**
       * just an abstract class requiring a delivery mechanism for a participant.
       */
      class offnao_participant {
         public:
            // TODO(jayen): move the function "bodies" into a cpp file
            virtual ~offnao_participant() {}
            virtual void deliver(Blackboard *blackboard) = 0;
      };

      typedef boost::shared_ptr<offnao_participant> offnao_participant_ptr;

      /**
       * a "room" to keep track of all the "participants" and the message to send
       */
      class offnao_room {
         public:
            /**
             * adds a "participant" to this "room"
             *
             * @param participant the participatnt to add
             */
            void join(offnao_participant_ptr participant);

            /**
             * removes a "participant" to this "room"
             *
             * @param participant the participatnt to remove
             */
            void leave(offnao_participant_ptr participant);

            /**
             * delivers a message to all the participants
             *
             * @param blackboard the message to send
             */
            void deliver(Blackboard *blackboard);

         private:
            std::set<offnao_participant_ptr> participants_;
      };

      /**
       * A debugging session.  Contains a series of messages and a "room"
       */
      class offnao_session :
         public offnao_participant,
         public boost::enable_shared_from_this<offnao_session> {
         public:
            /**
             * constructor.
             *
             * @param io_service the io_service associated with this session
             * @param room the "room" associated with this session
             */
            offnao_session(boost::asio::io_service* io_service,
                           offnao_room* room);

            /**
             * @return the socket associated with this session
             */
            boost::asio::ip::tcp::socket& socket();

            /**
             * starts this session.  binds reads to be handled
             */
            void start(Blackboard *blackboard);

            /**
             * delivers messages to this session by binding an asynchronous write
             *
             * @param blackboard the message to be delivered
             */
            void deliver(Blackboard *blackboard);

            /**
             * handles reads.  sets some internal variables to control what to send.
             *
             * @param error an error, if there was one during reading
             * @param blackboard the blackboard read
             */
            void handle_read(const boost::system::error_code& error, Blackboard *blackboard);

            /**
             * handles writes.  should send packets every tick by scheduling an async
             * write
             *
             * @param error i don't know... :(
             */
            void handle_write(const boost::system::error_code& error);

         private:
            Connection connection_;
            offnao_room& room_;
            /**
             * the mask received from offnao.  can be a command or a data request
             */
            OffNaoMask_t receivedMask;
            /**
             * the mask to send to offnao.  should only be a data mask
             */
            OffNaoMask_t sendingMask;
      };

      typedef boost::shared_ptr<offnao_session> offnao_session_ptr;

      /**
       * starts a session, and rebinds itself for another connection
       */
      void handle_accept(offnao_session_ptr session,
                         const boost::system::error_code& error);

      offnao_room room_;

      /**
       * the acceptor
       */
      boost::asio::ip::tcp::acceptor* acceptor_;
};
