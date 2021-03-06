//
//  test_trackerimpl.cpp
//  unittests
//
//  Created by Nathan Holmberg on 25/03/13.
//  Copyright (c) 2013 nshgraph. All rights reserved.
//

#include "gtest/gtest.h"
#include "gai++/Transaction.h"

#include "TrackerImpl.h"
#include "HitStore.h"
#include "Hit.h"


class FakeHitStore : public GAI::HitStore
{
public:
    FakeHitStore()
    {
    }
    bool storeHit( const GAI::Hit& hit )
    {
        mHits.push_back(hit);
        return true;
    }
    int getNumHits()
    {
        return mHits.size();
    }
    
    GAI::Hit& getHit(int index)
    {
        return mHits[index];
    }
private:
    std::vector<GAI::Hit> mHits;
};

void ensureHitHasParameter(const GAI::Hit& hit, std::string key, std::string value)
{
    std::string url = hit.getDispatchURL();
    std::string test_string = key + "=" + value;
    EXPECT_TRUE(url.find(test_string) != std::string::npos);
    
}

TEST( TrackerImplTest, send_types )
{
    FakeHitStore dispatch = FakeHitStore();
    GAI::TrackerImpl tracker = GAI::TrackerImpl(dispatch,"clientID","trackingID","appName","appVersion");
    
    EXPECT_EQ(dispatch.getNumHits(),0);
    
    // attempt to send a view
    EXPECT_TRUE( tracker.sendView("screen") );
    EXPECT_EQ(dispatch.getNumHits(),1);
    
    EXPECT_FALSE( tracker.sendView("") );
    EXPECT_EQ(dispatch.getNumHits(),1);
    
    // attempt to send an event
    EXPECT_TRUE( tracker.sendEvent("category","action","label") );
    EXPECT_EQ(dispatch.getNumHits(),2);
    
    // attempt to send an exception
    EXPECT_TRUE( tracker.sendException(false,"exception") );
    EXPECT_EQ(dispatch.getNumHits(),3);
    EXPECT_TRUE( tracker.sendException(true,"exception") );
    EXPECT_EQ(dispatch.getNumHits(),4);
    
    GAI::Transaction* transaction = GAI::Transaction::createTransaction("id", "affiliation");
    EXPECT_TRUE( tracker.sendTransaction(transaction) );
    EXPECT_EQ(dispatch.getNumHits(),5);
    delete( transaction );
    
    // attempt to send timing
    EXPECT_TRUE( tracker.sendTimingWithCategory("category", 1.0, "name", "label" ) );
    EXPECT_EQ(dispatch.getNumHits(),6);
    
    // attempt to send social
    EXPECT_TRUE( tracker.sendSocial("network","action","target") );
    EXPECT_EQ(dispatch.getNumHits(),7);
    
    // can close
    tracker.close();
    
    EXPECT_FALSE( tracker.sendSocial("network","action","target") );
    EXPECT_EQ(dispatch.getNumHits(),7);
}

TEST( TrackerImplTest, get_and_set)
{
    FakeHitStore dispatch = FakeHitStore();
    std::string clientID = "clientID";
    std::string trackingID = "trackingID";
    std::string appName = "appName";
    std::string appName2 = "appName2";
    std::string appVersion = "appVersion";
    std::string appVersion2 = "appVersion2";
    std::string appID = "appID";
    std::string appID2 = "appID2";
    std::string referrer = "referrer";
    std::string campaign = "campaign";
    GAI::TrackerImpl tracker = GAI::TrackerImpl(dispatch,clientID.c_str(),trackingID.c_str(),appName.c_str(),appVersion.c_str());
    
    // can get tracker id
    EXPECT_EQ( tracker.getTrackingId(), trackingID );
    
    // can get tracker id
    EXPECT_EQ( tracker.getClientId(), clientID );
    
    // can get and set app name
    EXPECT_EQ( tracker.getAppName(), appName );
    
    tracker.setAppName(appName2.c_str());
    EXPECT_EQ( tracker.getAppName(), appName2 );
    
    // can get and set app version
    EXPECT_EQ( tracker.getAppVersion(), appVersion );
    
    tracker.setAppVersion(appVersion2.c_str());
    EXPECT_EQ( tracker.getAppVersion(), appVersion2 );
    
    // can get and set app ID
    tracker.setAppId(appID.c_str());
    EXPECT_EQ( tracker.getAppId(), appID );
    
    // can get and set anonymize
    tracker.setAnonymize(false);
    EXPECT_EQ( tracker.isAnonymize(), false );
    tracker.setAnonymize(true);
    EXPECT_EQ( tracker.isAnonymize(), true );
    
    // can get and set sample rate
    tracker.setSampleRate(1.0);
    EXPECT_EQ( tracker.getSampleRate(), 1.0 );
    tracker.setSampleRate(2.0);
    EXPECT_EQ( tracker.getSampleRate(), 2.0 );
    
    // can get and set sample rate
    tracker.setSessionTimeout(1.0);
    EXPECT_EQ( tracker.getSessionTimeout(), 1.0 );
    tracker.setSessionTimeout(2.0);
    EXPECT_EQ( tracker.getSessionTimeout(), 2.0 );
    
    // can get and set refferer
    tracker.setReferrerUrl(referrer.c_str());
    EXPECT_EQ( tracker.getReferrerUrl(), std::string(referrer) );
    
    // can get and set campaignid
    tracker.setCampaignUrl(campaign.c_str());
    EXPECT_EQ( tracker.getCampaignUrl(), campaign );
    
    // if we create a hit and start a session we can't change the app info anymore
    EXPECT_TRUE( tracker.sendView("screen") );
    
    
    tracker.setAppVersion(appVersion.c_str());
    EXPECT_EQ( tracker.getAppVersion(), appVersion2 ); // no change!
    
    tracker.setAppName(appName.c_str());
    EXPECT_EQ( tracker.getAppName(), appName2 ); // no change!
    
    tracker.setAppId(appID2.c_str());
    EXPECT_EQ( tracker.getAppId(), appID ); // no change!
}

TEST( TrackerImplTest, custom_metrics_and_dimensions )
{
    FakeHitStore dispatch = FakeHitStore();
    GAI::TrackerImpl tracker = GAI::TrackerImpl(dispatch,"clientID","trackingID","appName","appVersion");
    
    EXPECT_EQ(dispatch.getNumHits(),0);
    
    
    // attempt to send an event
    GAI::CustomDimensionMap dimensions;
    GAI::CustomMetricMap metrics;
    dimensions[1] = "test";
    metrics[1] = "5";
    
    EXPECT_TRUE( tracker.sendEvent("category","action","label",0,dimensions,metrics) );
    EXPECT_EQ(dispatch.getNumHits(),1);

    // retrieve the event and ensure it has the dimension and metric
    GAI::Hit& hit = dispatch.getHit(0);
    ensureHitHasParameter(hit,"cd1","test");
    ensureHitHasParameter(hit,"cm1","5");
    
}