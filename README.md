# Battlesnake

## Run Locally

Build:

```bash
docker build -t battlesnake .
```

Run:

```bash
docker run --rm -p 8080:8080 battlesnake
```

Test:

```bash
curl http://localhost:8080/
```

## Redeploy to AWS ECS Fargate

Load local deployment values:

```bash
source .env
```

Build and push a new ARM64 image to ECR:

```bash
docker buildx build \
  --platform linux/arm64 \
  -t $ECR_IMAGE \
  --push .
```

Restart the ECS service so Fargate pulls the new image:

```bash
aws ecs update-service \
  --cluster $ECS_CLUSTER \
  --service $ECS_SERVICE \
  --force-new-deployment
```

The Fargate public IP may change after redeploying.
