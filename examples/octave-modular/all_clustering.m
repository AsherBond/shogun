init_shogun

% Explicit examples on how to use clustering

addpath('tools');
fm_train=load_matrix('../data/fm_train_real.dat');


% KMeans
disp('KMeans')

k=4;
feats_train=RealFeatures(fm_train);
feats_test=RealFeatures(fm_train);
distance=EuclidianDistance(feats_train, feats_train);

kmeans=KMeans(k, distance);
kmeans.train();

distance.init(feats_train, feats_test);
c=kmeans.get_cluster_centers();
r=kmeans.get_radiuses();

% Hierarchical
disp('Hierarchical')

merges=4;
feats_train=RealFeatures(fm_train);
feats_test=RealFeatures(fm_train);
distance=EuclidianDistance(feats_train, feats_train);

hierarchical=Hierarchical(merges, distance);
hierarchical.train();

distance.init(feats_train, feats_test);
mdist=hierarchical.get_merge_distances();
pairs=hierarchical.get_cluster_pairs();