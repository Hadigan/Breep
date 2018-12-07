///////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                               //
// Copyright 2017-2018 Lucas Lazare.                                                             //
// This file is part of Breep project which is released under the                                //
// European Union Public License v1.1. If a copy of the EUPL was                                 //
// not distributed with this software, you can obtain one at :                                   //
// https://joinup.ec.europa.eu/community/eupl/og_page/european-union-public-licence-eupl-v11     //
//                                                                                               //
///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @file local_peer.tcc
 * @author Lucas Lazare
 * @since 0.1.0
 */


template <typename T>
inline const typename T::peer*& breep::local_peer<T>::path_to(const typename T::peer& p) {
	return m_path_to_passing_by.at(p.id());
}

template <typename T>
inline typename T::peer const * const & breep::local_peer<T>::path_to(const typename T::peer& p) const {
	return m_path_to_passing_by.at(p.id());
}

template <typename T>
inline std::unordered_map<boost::uuids::uuid, const typename T::peer*, boost::hash<boost::uuids::uuid>>& breep::local_peer<T>::path_to_passing_by() noexcept {
	return m_path_to_passing_by;
}

template <typename T>
inline const std::unordered_map<boost::uuids::uuid, const typename T::peer*, boost::hash<boost::uuids::uuid>>& breep::local_peer<T>::path_to_passing_by() const noexcept {
	return m_path_to_passing_by;
}

template <typename T>
inline std::unordered_map<boost::uuids::uuid, std::vector<const typename T::peer*>, boost::hash<boost::uuids::uuid>>& breep::local_peer<T>::bridging_from_to() noexcept {
	return m_bridging_from_to;
}

template <typename T>
const std::unordered_map<boost::uuids::uuid, std::vector<const typename T::peer*>, boost::hash<boost::uuids::uuid>>& breep::local_peer<T>::bridging_from_to() const noexcept {
	return m_bridging_from_to;
}