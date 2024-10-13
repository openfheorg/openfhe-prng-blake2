// clang-format off
//==================================================================================
// BSD 2-Clause License
//
// Copyright (c) 2014-2022, NJIT, Duality Technologies Inc. and other contributors
//
// All rights reserved.
//
// Author TPOC: contact@openfhe.org
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//==================================================================================
/*
  PRNG engine based on BLAKE2b
 */

#ifndef __BLAKE2ENGINE_H__
#define __BLAKE2ENGINE_H__

#include "prng.h"

#include <array>
#include <mutex>

/**
 * @brief Defines the PRNG engine (based on BLAKE2) used by OpenFHE. It can be used
 * as an example for adding other PRNG engines to OpenFHE.
 */
class Blake2Engine : public PRNG {
 public:
  /**
   * @brief Constructor using a small seed - used for generating a large seed
   */
  // TODO (dsuponit): do we need "explicit Blake2Engine(PRNG::result_type seed)"? Only used in UnitTestTransform.cpp:l.983?
//   explicit Blake2Engine(PRNG::result_type seed)
//       : PRNG(seed), m_counter(0), m_buffer({}), m_bufferIndex(0) {
//     m_seed[0] = seed;
//   }

  /**
   * @brief Main constructor taking a vector of MAX_SEED_GENS integers as a seed and a counter.
   *        If there is no value for the counter, then pass zero as the counter value
   */
  explicit Blake2Engine(const std::array<PRNG::result_type, PRNG::MAX_SEED_GENS>& seed,
                        PRNG::result_type counter)
      : PRNG(seed, counter), m_buffer({}), m_bufferIndex(0) {}

  /**
   * @brief main call to the PRNG
   */
  PRNG::result_type operator()() override {
      if (m_bufferIndex == static_cast<uint16_t>(PRNG::PRNG_BUFFER_SIZE)) 
          m_bufferIndex = 0;

      // makes a call to the BLAKE2 generator only when the currently buffered values are all consumed precomputations and
      // done only once for the current buffer
      if (m_bufferIndex == 0)
          Generate();

      PRNG::result_type result = m_buffer[m_bufferIndex];
      m_bufferIndex++;

      return result;
  }

  Blake2Engine(const Blake2Engine& other)
    : PRNG(other), m_buffer(other.m_buffer), m_bufferIndex(other.m_bufferIndex) {}

  // TODO (dsuponit): do we need "operator=" which returns void instead of Blake2Engine& (*this)?
  // TODO (dsuponit): do we need it at all?
//   void operator=(const Blake2Engine& other) {
//       m_counter = other.m_counter;
//       m_seed = other.m_seed;
//       m_buffer = other.m_buffer;
//       m_bufferIndex = other.m_bufferIndex;
//   }

 private:
    /**
     * @brief The main call to blake2xb function
     */
    void Generate();

    // The vector that stores random samples generated using the hash function
    std::array<PRNG::result_type, PRNG::PRNG_BUFFER_SIZE> m_buffer{};

    // Index in m_buffer corresponding to the current PRNG sample
    uint16_t m_bufferIndex = 0;

    std::mutex mtx;
};

/**
 * @brief createEngineInstance() generates a Blake2Engine object which is dynamically allocated
 * @return pointer to the generated Blake2Engine object
 * @attention the caller is responsible for freeing the memory allocated by this function 
 **/
extern "C" {
    Blake2Engine* createEngineInstance(const std::array<PRNG::result_type, PRNG::MAX_SEED_GENS>& seed,
                                       PRNG::result_type counter) {
      return new Blake2Engine(seed, counter);
    }
}

#endif
// clang-format on