/* Copyright (c) 2015-2017, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <brayns/Brayns.h>

#include <brayns/common/Timer.h>
#include <brayns/engine/Camera.h>
#include <brayns/engine/Engine.h>
#include <brayns/engine/FrameBuffer.h>
#include <brayns/engine/Renderer.h>
#include <brayns/engine/Scene.h>
#include <brayns/parameters/ParametersManager.h>

#define BOOST_TEST_MODULE brayns
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(default_scene_benckmark)
{
    auto& testSuite = boost::unit_test::framework::master_test_suite();
    brayns::Brayns brayns(testSuite.argc,
                          const_cast<const char**>(testSuite.argv));

    uint64_t reference, shadows, softShadows, ambientOcclusion, allOptions;

    // Set default rendering parameters
    brayns::ParametersManager& params = brayns.getParametersManager();
    params.getRenderingParameters().setSamplesPerPixel(32);
    brayns.commit();

    // Start timer
    brayns::Timer timer;
    timer.start();
    brayns.render();
    timer.stop();
    reference = timer.milliseconds();

    auto& renderer = brayns.getEngine().getRenderer();

    // Shadows
    auto props = renderer.getPropertyMap();
    props.updateProperty("shadows", 1.);
    renderer.updateProperties(props);
    brayns.commit();

    timer.start();
    brayns.render();
    timer.stop();
    shadows = timer.milliseconds();

    // Shadows
    float t = float(shadows) / float(reference);
    BOOST_TEST_MESSAGE("Shadows cost. expected: 165%, realized: " << t * 100.f);
    BOOST_CHECK(t < 1.65f);

    props.updateProperty("softShadows", 1.);
    renderer.updateProperties(props);
    brayns.commit();

    timer.start();
    brayns.render();
    timer.stop();
    softShadows = timer.milliseconds();

    // Soft shadows
    t = float(softShadows) / float(reference);
    BOOST_TEST_MESSAGE(
        "Soft shadows cost. expected: 185%, realized: " << t * 100.f);
    BOOST_CHECK(t < 1.85f);

    // Ambient occlustion
    props.updateProperty("shadows", 0.);
    props.updateProperty("softShadows", 0.);
    props.updateProperty("aoWeight", 1.);
    renderer.updateProperties(props);
    brayns.commit();

    timer.start();
    brayns.render();
    timer.stop();
    ambientOcclusion = timer.milliseconds();

    // Ambient occlusion
    t = float(ambientOcclusion) / float(reference);
    BOOST_TEST_MESSAGE(
        "Ambient occlusion cost. expected: 250%, realized: " << t * 100.f);
    BOOST_CHECK(t < 2.5f);

    // All options
    props.updateProperty("shadows", 1.);
    props.updateProperty("softShadows", 1.);
    props.updateProperty("aoWeight", 1.);
    renderer.updateProperties(props);
    brayns.commit();

    timer.start();
    brayns.render();
    timer.stop();
    allOptions = timer.milliseconds();

    // All options
    t = float(allOptions) / float(reference);
    BOOST_TEST_MESSAGE(
        "All options cost. expected: 350%, realized: " << t * 100.f);
    BOOST_CHECK(t < 3.5f);
}
