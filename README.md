# Eventflow - A Message Broker System

Eventflow is an experimental message broker system, developed to understand the working of a message broker. In essence, Eventflow captures the basic features of enterprise event stream processing softwares like [Apache Kafka](https://kafka.apache.org/) or [RabbitMQ](https://www.rabbitmq.com/). Eventflow is also similar to the jobs queue use case of [Redis](https://redis.io/). This system is developed purely for learning purposes and not suitable for production. Go through this README to learn more about message brokers and its use cases. Instructions to setup Eventflow in your own device, and implementation details of Eventflow's API are also provided. 

Contents:

- [What is a message broker?](#messagebroker)
- [Use Cases](#usecases)
- [Features](#features)
- [Eventflow API](#api)
- [Setup](#setup)
- [Limitations](#limitations)
- [Developers](#dev)
- [References](#references)

## <a name="messagebroker"></a>What is a message broker?

A message broker is a system that facilitates the communication between different applications, services and system. In a production environment, message brokers effectively translate messages between formal messaging protocols. It hence allows for easy flow of data between applications and services, even if they're written in different languages and follow different protocols.

For a brief introduction on message brokers, check out [this video](https://www.youtube.com/watch?v=wA259esVY4A).

#### Why is it needed?

In complex applications where several services are running interdependent to each other, each service must be connected with every other service. Without a message broker middleware, the connections essentially form a K-complete graph, where each vertex represents a service. 

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<img src="./images/K3K7.jpg" alt="k3k7" width="50%"/>

As we can see, the number of connections quickly increases with increase in number of services. Forming these connections seperately is a costly and complicated operation. That's where a message broker comes into the picture. Instead of connecting all the interdependent applications to each other, they're connected to a message broker. This acts as a point of communication. Services can send messages to a the message broker. These messages stay there until a receiving application is ready to fetch them. Now all applications are just connected to the message broker, they do not need to know the destination to send messages. 

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<img style="left:33%" src="./images/messagebroker1.png" alt="messagebroker" width="50%"/>

The two basic messaging styles of a message broker system are:

1. Point-to-point messaging
2. Publish/Subscribe messaging

Further information about these distribution patterns is provided in [Features](#features).

## <a name="usecases"></a>Use Cases
## <a name="features"></a>Features

Eventflow offers implementation of the two message distribution patterns, point-to-point messaging and publish/subscribe messaging. Additionally, status logging is also provided.

1. <b>Publish/Subscribe messaging - </b>This pattern, often referred to as pub/sub is used for broadcast style communication. The producer of a message publishes it to a topic. A consumer subscribes to a topic to receive its messages. All applications subcribed to this topic will receive all the messages published on to it. This establishes a one-to-many relationship between the producer and consumers of message. For example, in the case of change in route of a train, this information may be useful to several entities, the staff positioned at railway stations concerned by the update, the locomotive pilot and the staff onboard the train, and the passengers. In this case, pub/sub model should be used.

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<img src="./images/pubsub.png" alt="publish/subscribe pattern" width="50%"/>

&nbsp;&nbsp;&nbsp; Note that a publisher can also subscribe to topics and vice versa. 

2. <b>Point-to-Point messaging - </b>This pattern is used when there is a one-to-one relationship between producer and consumer. Only one producer will send messages to a message queue. These messages will be fetched by only one consumer, and only once. This pattern is useful in financial transactions, where a payment should be carried out exactly once. 

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<img src="./images/P2P.png" alt="point-to-point pattern" width="50%"/>

3. The status log of any activity that occurs after connection with Eventflow is established is logged in a separate topic defined for logging. The log includes details about the activity and the time it occured. Steps to define a topic for status log are listed in [Setup](#setup). 

Eventflow API contains publish and subscribe for achieving publish/subscribe messaging. Additionally, client authentication and authorization features are provided for point-to-point messaging. However, acks have not been implemented i.e. a producer will never know if a message is delivered. This bears a serious limitation in message communication. More on this in [Eventflow API](#api).

## <a name="api"></a>Eventflow API
## <a name="setup"></a>Setup
## <a name="limitations"></a>Limitations
## <a name="dev"></a>Developers
## <a name="references"></a>References

[IBM - Message Brokers](https://www.ibm.com/cloud/learn/message-brokers) - For detailed information on message brokers.

[Making Sense of Stream Processing](https://assets.confluent.io/m/2a60fabedb2dfbb1/original/20190307-EB-Making_Sense_of_Stream_Processing_Confluent.pdf) - An excellent book by Martin Kleppmann about the philosophy behind Scalable Stream Data Platforms.
