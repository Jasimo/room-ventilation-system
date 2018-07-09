/*
 * Copyright (C) 2018 Sven Just (sven@familie-just.de)
 * Copyright (C) 2018 Ivan Schréter (schreter@gmx.net)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * This copyright notice MUST APPEAR in all copies of the software!
 */

/*!
 * @file
 * @brief Client to communicate with MQTT protocol.
 */
#pragma once

#include "StringView.h"
#include "Task.h"

#include <Ethernet.h>
#include <PubSubClient.h>

/*!
 * @brief Client to communicate with MQTT protocol over Ethernet.
 */
class NetworkClient : private Task
{
public:
  NetworkClient(const NetworkClient&) = delete;
  NetworkClient& operator=(const NetworkClient&) = delete;

  /// Construct network client.
  explicit NetworkClient(Scheduler& scheduler);

  /// Start network client.
  void start(Print& initTracer);

  /// Check if the global MQTT client is already installed.
  static bool hasClient() { return s_client_ != nullptr; }

  /// Get global instance of the MQTT client.
  static PubSubClient& getClient() { return *s_client_; }

  /// Check if LAN is OK.
  bool isLANOk() const { return lan_ok_; }

  /// Check if MQTT is OK.
  bool isMQTTOk() const { return mqtt_ok_; }

private:
  /// Initialize Ethernet connection.
  void initEthernet();

  /// Initialize MQTT connection.
  bool mqttConnect();

  virtual bool poll() override;

  virtual void run() override;

  /// Static link to the client to be used by MessageHandler to publish messages.
  static PubSubClient* s_client_;

  /// Scheduler scheduling tasks.
  Scheduler& scheduler_;
  /// Ethernet client for MQTT client.
  EthernetClient eth_client_;
  /// MQTT client.
  PubSubClient mqtt_client_;
  /// Last time when LAN started a reconnect attempt.
  unsigned long last_mqtt_reconnect_attempt_time_ = 0;
  /// Last time when MQTT started a reconnect attempt.
  unsigned long last_lan_reconnect_attempt_time_ = 0;
  /// Flag set when LAN is present.
  bool lan_ok_ = false;
  /// Flag set when MQTT is present.
  bool mqtt_ok_ = false;
  /// Subscribe flag for commands.
  bool subscribed_command_ = false;
  /// Subscribe flag for debug commands.
  bool subscribed_debug_ = false;
};
