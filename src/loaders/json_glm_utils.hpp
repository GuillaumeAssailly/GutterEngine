#ifndef JSON_GLM_UTILS_HPP
#define JSON_GLM_UTILS_HPP

#include "json.hpp"
#include"../config.h"

using json = nlohmann::json;

namespace glm {
    template <typename T, glm::length_t L>
    void to_json(json& j, const glm::vec<L, T>& v) {
        j = json::array();
        for (glm::length_t i = 0; i < L; ++i) {
            j.push_back(v[i]);
        }
    }

    template <typename T, glm::length_t L>
    void from_json(const json& j, glm::vec<L, T>& v) {
        if (j.is_array() && j.size() == L) {
            for (glm::length_t i = 0; i < L; ++i) {
                v[i] = j.at(i).get<T>();
            }
        }
        else {
            throw std::runtime_error("JSON array size does not match glm::vec length");
        }
    }

    template <typename T>
    struct is_glm_quat : std::false_type {};

    template <typename T, glm::qualifier Q>
    struct is_glm_quat<glm::qua<T, Q>> : std::true_type {};

    template <typename QuatType, typename std::enable_if<is_glm_quat<QuatType>::value, int>::type = 0>
    void to_json(json& j, const QuatType& q) {
        j = json::array({ q.w, q.x, q.y, q.z });
    }

    template <typename QuatType, typename std::enable_if<is_glm_quat<QuatType>::value, int>::type = 0>
    void from_json(const json& j, QuatType& q) {
        if (j.is_array() && j.size() == 4) {
            q.w = j.at(0).get<typename QuatType::value_type>();
            q.x = j.at(1).get<typename QuatType::value_type>();
            q.y = j.at(2).get<typename QuatType::value_type>();
            q.z = j.at(3).get<typename QuatType::value_type>();
        }
        else {
            throw std::runtime_error("JSON array size does not match glm::quat length");
        }
    }

    template <typename T, glm::length_t C, glm::length_t R, glm::qualifier Q>
    void to_json(json& j, const glm::mat<C, R, T, Q>& m) {
        j = json::array();
        for (glm::length_t col = 0; col < C; ++col) {
            for (glm::length_t row = 0; row < R; ++row) {
                j.push_back(m[col][row]);
            }
        }
    }

    template <typename T, glm::length_t C, glm::length_t R, glm::qualifier Q>
    void from_json(const json& j, glm::mat<C, R, T, Q>& m) {
        for (glm::length_t col = 0; col < C; ++col) {
            for (glm::length_t row = 0; row < R; ++row) {
                m[col][row] = j.at(col * R + row).get<T>();
            }
        }
    }

}




#endif
